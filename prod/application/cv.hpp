#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "LibCamera.h"
#include "interface.hpp"
#include "utility.hpp"

namespace app {

// class Camera final {
// public:
//     Camera();
// private:
//     std::shared_ptr<libcamera::Camera> camera_;
//     std::unique_ptr<libcamera::CameraManager> camera_manager_;
//     std::unique_ptr<libcamera::CameraConfiguration> config_;
// };

class CVision: public IProcessing {
public:
    explicit CVision(const std::string&);
    ~CVision();
    bool process() override;
private:
    // cv::VideoCapture capture_;
    // cv::Mat frame_;
    LowPassFilter filter_;


    // uint32_t width = 640;
    // uint32_t height = 480;
    // uint32_t stride = 0;
    // char key;
    // int window_width = 640;
    // int window_height = 480;
    // LibCamera cam;

    // LibcameraOutData frameData;
};

} //namespace app

#endif //CV_H