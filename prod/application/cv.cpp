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
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");

    cv::namedWindow( "w", 1);
}

bool CameraProcessingModule::process()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return false;
        cv::imshow("w", frame_);
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
        
        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        process_params_.filtered = filter_.filter(process_params_.brightness);

        // if(calc_params_.event_completeness.calibration) {
        //     process_params_.filtered -= calc_params_.mean_filtered;
        //     process_params_.brightness -= calc_params_.mean_filtered;
        // }

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