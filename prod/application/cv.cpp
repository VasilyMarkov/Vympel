#include <numeric>
#include "cv.hpp"
#include "configReader.hpp"
#include <filesystem>

namespace app
{
    
using namespace constants;
namespace fs = std::filesystem;

CameraProcessingModule::CameraProcessingModule():
    capture_((fs::current_path().parent_path() / configReader.get("files", "videoFile").toString().toStdString())), 
    filter_(filter::cutoff_frequency, filter::sample_rate)
{

    // if(!capture_.isOpened()) throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);

    // if (width > window_width)
    // {
    //     cv::namedWindow("libcamera-demo", cv::WINDOW_NORMAL);
    //     cv::resizeWindow("libcamera-demo", window_width, window_height);
    // } 

    int ret = cam.initCamera();
    cam.configureStill(width, height, formats::RGB888, 1, 0);
    
    int64_t frame_time = 1000000 / 60;
    controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    cam.set(controls_);
    
    cam.startCamera();
    cam.VideoStream(&width, &height, &stride);
}

bool CameraProcessingModule::process()
{
    if (cam.readFrame(&frameData)) {
        Mat im(height, width, CV_8UC3, frameData.imageData, stride);

        // imshow("libcamera-demo", im);
        // key = waitKey(1);

        std::vector<uint8_t> v(im.begin<uint8_t>(), im.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        process_params_.filtered = filter_.filter(process_params_.brightness);

        // std::cout << process_params_.filtered << std::endl;
    }
    ++global_tick_;
    return true;
        
}

NetLogic::NetLogic():
    cameraSocket_(std::make_unique<UdpSocket>())
{
    connect(cameraSocket_.get(), &UdpSocket::sendData, this, &NetLogic::receiveData, Qt::QueuedConnection);

    cameraSocket_->setReceiverParameters(QHostAddress(configReader.get("network", "clientIp").toString()), 
                                         configReader.get("network", "videoPort").toInt());
}

std::optional<double> NetLogic::getValue()
{
    if(receiveBuffer_.empty()) return std::nullopt;

    auto value = receiveBuffer_.front();
    receiveBuffer_.pop();
    return value;
}

void NetLogic::receiveData(const QJsonDocument& json) {
    receiveBuffer_.push(json["brightness"].toDouble());
}

NetProcessing::NetProcessing(): filter_(filter::cutoff_frequency, filter::sample_rate) {}

bool NetProcessing::process()
{
    if(netLogic.getValue()) {
        process_params_.brightness = netLogic.getValue().value(); 
        process_params_.filtered = filter_.filter(process_params_.brightness);
    }
    ++global_tick_;
    return true;
}


} // namespace app
