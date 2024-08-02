#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include <list>
#include "io_interface.hpp"
#include <qt6/QtCore/QObject>

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


class Core final: public QObject, public ISubject {
    Q_OBJECT
public:
    Core(const std::string&);
    void attach(std::unique_ptr<IObserver>) override;
    void detach(std::unique_ptr<IObserver>) override;
public slots:
    void receiveData();
    void process();
private:
    void notify() const override;
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;
    params_t params_;
    std::list<std::unique_ptr<IObserver>> observers_;
signals:
    void sendData(const params_t&);
};

}

Q_DECLARE_METATYPE(app::params_t)
Q_DECLARE_METATYPE(app::core_mode_t)