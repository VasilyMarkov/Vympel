#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"

namespace app {

class CVision: public IProcessing {
public:
    explicit CVision(const std::string&);
    bool process() override;
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;
};

} //namespace app

#endif //CV_H