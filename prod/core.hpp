#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include <list>
#include "io_interface.hpp"
#include <qt6/QtCore/QObject>
#include <unordered_map>
#include <functional>

namespace app {

namespace constants {
    namespace filter {
        constexpr double cutoff_frequency = 10.0; //Hz
        constexpr double sample_rate = 1000.0; //Hz
    }
}

class LowPassFilter {
 public:
  LowPassFilter(float cutoff_frequency, float sample_rate, float q = 0.707)
      : alpha_(std::sin(2 * M_PI * cutoff_frequency / sample_rate) / (2 * q)),
        y_(0) {}

  float Process(float x) {
    y_ = alpha_ * (x - y_) + y_;
    return y_;
  }

 private:
  float alpha_;
  float y_;
};

class Event {
protected:
    params_t params_;
public:
    Event(params_t&);
    virtual void operator()(size_t) = 0;
    virtual ~Event(){}
};

class Idle final: public Event {
public:
    Idle(params_t&);
    void operator()(size_t) override;
};

class Calibration final: public Event {
    std::vector<double> data_;
public:
    Calibration(params_t&);
    void operator()(size_t) override;
};

class Measurement final: public Event {
    std::vector<double> data_;
public:
    Measurement(params_t&);
    void operator()(size_t) override;
};


class Fsm {
public:
    Fsm(params_t&, size_t);
    void toggle(const QString&);
    void callEvent();
private:
    void dispatchEvent();
private:
    params_t params_;
    size_t global_tick_ = 0;
    core_mode_t mode_ = core_mode_t::IDLE;
    std::unique_ptr<Event> active_event_ = nullptr;
    const std::unordered_map<QString, core_mode_t> events_;
};


class Core final: public QObject {
    Q_OBJECT
public:
    Core(const std::string&);
public slots:
    void receiveData(const QString&);
    void process();
private:
    bool computerVision();
signals:
    void sendData(const params_t&);
    void exit();
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;
    params_t params_;
    Fsm fsm_;
    size_t global_tick_ = 0;
};

}

Q_DECLARE_METATYPE(app::params_t)
Q_DECLARE_METATYPE(app::core_mode_t)