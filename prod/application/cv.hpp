#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
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