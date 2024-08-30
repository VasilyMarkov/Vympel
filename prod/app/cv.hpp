#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"
#undef signals
#undef slots
#undef emit
#undef foreach
#include "libcamera/libcamera.h"

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