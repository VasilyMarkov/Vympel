#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"
#include "udp.hpp"

namespace app {

class CameraProcessingModule: public IProcessing {
public:
    explicit CameraProcessingModule();
    bool process() override;
private:
    cv::VideoCapture capture_;
    cv::Mat frame_;
    LowPassFilter filter_;
};

class NetProcessing: public IProcessing {
    // Q_OBJECT
public:
    NetProcessing();
private slots:
    void receiveData(double);
private:
    std::unique_ptr<UdpSocket> socket_;
};

} //namespace app

#endif //CV_H