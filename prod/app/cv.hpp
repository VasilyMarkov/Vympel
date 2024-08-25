#include <opencv2/opencv.hpp>
#include "interface.hpp"

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

} //namespace app