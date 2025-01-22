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
#include "udp.hpp"
#include <queue>

namespace app {

class CameraProcessingModule: public IProcessing {
public:
    explicit CameraProcessingModule();
    state process() override;
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

struct dataCV {
    double value;
    bool valid;
};

class NetLogic: public QObject, IReceiver {
    Q_OBJECT
public:
    NetLogic();
    std::optional<dataCV> getValue();
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
private:
    std::queue<dataCV> receiveBuffer_;
    std::unique_ptr<UdpSocket> cameraSocket_;
};

class NetProcessing: public IProcessing {
public:
    NetProcessing();
private:
    state process() override;
    NetLogic netLogic;
    LowPassFilter filter_;
};

} //namespace app

#endif //CV_H