#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "LibCamera.h"
#include "interface.hpp"
#include "utility.hpp"

namespace app {

class CVision: public IProcessing {
public:
    explicit CVision(const std::string&);
    ~CVision();
    bool process() override;
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;


    LibCamera cam;
    uint32_t width = 640;
    uint32_t height = 480;
    uint32_t stride = 0;
    char key;
    int window_width = 640;
    int window_height = 480;
    LibcameraOutData frameData;
    ControlList controls_;
};

} //namespace app