#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"
#include "udp.hpp"
#include <queue>

namespace app {

class NetLogic: public QObject, IReceiver {
    Q_OBJECT
public:
    NetLogic();
    std::optional<double> getValue();
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
private:
    std::queue<double> receiveBuffer_;
    std::unique_ptr<UdpSocket> cameraSocket_;
};


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
public:
    NetProcessing();
private:
    bool process() override;
    NetLogic netLogic;
    LowPassFilter filter_;
};

} //namespace app

#endif //CV_H