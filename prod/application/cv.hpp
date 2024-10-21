#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"

/* For resolving names between Qt and libcamera. 
Uses instead Q_SIGNALS, Q_SLOTS, Q_EMIT, Q_FOREACH */
#undef signals
#undef slots
#undef emit
#undef foreach
/***************************************************/
#include "libcamera/libcamera.h"

namespace app {

class Camera final {
public:
    Camera();
private:
    std::shared_ptr<libcamera::Camera> camera_;
    std::unique_ptr<libcamera::CameraManager> camera_manager_;
    std::unique_ptr<libcamera::CameraConfiguration> config_;
};

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