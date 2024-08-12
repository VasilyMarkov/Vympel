#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <list>
#include "io_interface.hpp"
#include <qt6/QtCore/QObject>
#include <unordered_map>
#include <functional>
#include <optional>

namespace app {

namespace constants {
    namespace filter {
        constexpr double cutoff_frequency = 10.0; //Hz
        constexpr double sample_rate = 1000.0; //Hz
    }
    namespace buffer {
        constexpr size_t CALIB_SIZE = 200; 
        constexpr size_t MEASUR_SIZE = 200; 
    }
    constexpr size_t WAITING_TICKS = 100; 
    constexpr size_t FRAME_DELAY = 10; //ms
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

class CVision;

class Event {
protected:
    std::weak_ptr<CVision> cv_;
    std::vector<double> data_;
    size_t start_tick_ = 0;
public:
    Event(std::weak_ptr<CVision>);
    virtual std::optional<core_mode_t> operator()() = 0;
    virtual ~Event(){}
};

class Idle final: public Event {
public:
    Idle(std::weak_ptr<CVision>);
    ~Idle();
    std::optional<core_mode_t> operator()() override;
};

class Calibration final: public Event {
public:
    Calibration(std::weak_ptr<CVision>);
    ~Calibration();
    std::optional<core_mode_t> operator()() override;
};

class Measurement final: public Event {
public:
    Measurement(std::weak_ptr<CVision>);
    ~Measurement();
    std::optional<core_mode_t> operator()() override;
};


class Fsm final {
public:
    Fsm(std::weak_ptr<CVision>);
    void toggle(core_mode_t);
    void callEvent();
    void dispatchEvent();
private:
    core_mode_t mode_ = core_mode_t::IDLE;
    std::weak_ptr<CVision> cv_;
    std::unique_ptr<Event> active_event_ = nullptr;
};


class IProcessing{
protected:
    cv_params_t cv_params_;                    //parameters obtained by machine vision algorithm 
    calc_params_t calc_params_;                //parameters obtained by processing cv parameters inside events
    size_t global_tick_ = 0;
public:
    virtual bool process() = 0;
    virtual size_t getTick() const noexcept = 0;
    virtual cv_params_t getCvParams() const noexcept = 0;
    virtual calc_params_t& getCalcParams() noexcept = 0;
    virtual ~IProcessing(){}
};

class ICommunication {
public:
    virtual void receiveData(const QString&) = 0;
    virtual void sendData(const cv_params_t&) const = 0;;
    virtual ~ICommunication(){}
};

class CVision: public IProcessing {
public:
    explicit CVision(const std::string&);
    size_t getTick() const noexcept override;
    cv_params_t getCvParams() const noexcept override;
    calc_params_t& getCalcParams() noexcept override;
    bool process() override;
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;
};

class Core final: public QObject, public ICommunication {
    Q_OBJECT
public:
    explicit Core(const std::string&);
public slots:
    void receiveData(const QString&) override;
    bool process();
signals:
    void sendData(const cv_params_t&) const override;
    void exit();
private:
    std::shared_ptr<CVision> cv_ = nullptr;
    std::unique_ptr<Fsm> fsm_ = nullptr;
    
    const std::unordered_map<QString, core_mode_t> events_;
};

}

Q_DECLARE_METATYPE(app::cv_params_t)
Q_DECLARE_METATYPE(app::core_mode_t)
