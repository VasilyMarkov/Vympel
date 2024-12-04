#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "utility.hpp"
#include "udp.hpp"
#include <queue>

namespace app {

class NetLogic: public QObject, ICommunication {
    Q_OBJECT
public:
    NetLogic();
    std::optional<double> getValue() noexcept;
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
    void sendData(const process_params_t&) const override;
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
};

} //namespace app

#endif //CV_H