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
#include "network.hpp"
#include <queue>
#include <lccv.hpp>
#include <libcamera_app.hpp>


namespace app {

class CameraProcessingModule: public IProcessing {
public:
    explicit CameraProcessingModule();
    state process() override;
    const std::vector<uint8_t>& getBuffer() const noexcept {
        return buffer_;
    }
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
    LibcameraOutData frameData_;
    ControlList controls_;

    lccv::PiCamera cam_;
    std::vector<uint8_t> buffer_;
};

class TestCameraProcessingModule: public IProcessing {
public:
    explicit TestCameraProcessingModule();
    state process() override;
private:
    std::unique_ptr<CameraManager> camera_manager_;
    
    // std::unique_ptr<CameraConfiguration> camera_configuration_;
    // FrameBufferAllocator allocator_;
    // std::vector<std::unique_ptr<Request>> requests_;
    // std::unique_ptr<Request> request_;
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
    std::unique_ptr<UdpHandler> cameraSocket_;
};


class NetProcessUnit: public IProcessing {
public:
    NetProcessUnit();
private:
    state process() override;
    NetLogic netLogic;
    LowPassFilter filter_;
};

class TestProcessModule final: public IProcessing {
    std::vector<double> test_data_;
    LowPassFilter filter_;
public:
    TestProcessModule();
    state process() override;
};

} //namespace app

#endif //CV_H