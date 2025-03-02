#include <numeric>
#include "cv.hpp"
#include "configReader.hpp"
#include <filesystem>

namespace app
{
    
using namespace constants;
namespace fs = std::filesystem;

CameraProcessingModule::CameraProcessingModule():
    capture_((fs::current_path().parent_path() / ConfigReader::getInstance().get("files", "videoFile").toString().toStdString())), 
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

IProcessing::state CameraProcessingModule::process()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return IProcessing::state::NODATA;
        // cv::imshow("w", frame_);
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
        
        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        process_params_.filtered = filter_.filter(process_params_.brightness);
        std::cout << process_params_.brightness << std::endl;
        // if(calc_params_.event_completeness.calibration) {
        //     process_params_.filtered -= calc_params_.mean_filtered;
        //     process_params_.brightness -= calc_params_.mean_filtered;
        // }

        ++global_tick_;
        
        return IProcessing::state::WORKING;
        
}

NetLogic::NetLogic():
    cameraSocket_(std::make_unique<UdpHandler>())
{
    connect(cameraSocket_.get(), &UdpHandler::sendData, this, &NetLogic::receiveData, Qt::QueuedConnection);

    cameraSocket_->setReceiverParameters(QHostAddress(ConfigReader::getInstance().get("network", "clientIp").toString()), 
                                         ConfigReader::getInstance().get("network", "videoPort").toInt());
}

void NetLogic::receiveData(const QJsonDocument& json) {
    receiveBuffer_.push({json["brightness"].toDouble(), json["valid"].toBool()});
}

std::optional<dataCV> NetLogic::getValue()
{
    if(receiveBuffer_.empty()) return std::nullopt;

    auto value = receiveBuffer_.front();
    receiveBuffer_.pop();
    return value;
}

NetProcessUnit::NetProcessUnit(): filter_(filter::cutoff_frequency, filter::sample_rate) {}

IProcessing::state NetProcessUnit::process()
{   
    if(auto net_value = netLogic.getValue(); net_value.has_value()) {
        if(!net_value.value().valid) return IProcessing::state::DONE;

        process_params_.brightness = net_value.value().value; 
        process_params_.filtered = filter_.filter(process_params_.brightness);

        ++global_tick_;
        return IProcessing::state::WORKING;
    }
    else {
        return IProcessing::state::NODATA;
    }
}

TestProcessUnit::TestProcessUnit():
    test_data_(
        readJsonLog(
            fs::path(
                fs::current_path().parent_path() / "scripts" / "data/"
            ),
            "log1"
        )
    ), 
    filter_(filter::cutoff_frequency, filter::sample_rate) {
        btFilter_.setup (ConfigReader::getInstance().get("parameters", "sampling_rate_filter_freq_Hz").toInt(), 
            ConfigReader::getInstance().get("parameters", "filter_cutoff_freq_Hz").toInt());
    }

IProcessing::state TestProcessUnit::process() 
{
    if(global_tick_ == test_data_.size()) return IProcessing::state::DONE;

    process_params_.brightness = test_data_[global_tick_];
    // process_params_.filtered = filter_.filter(process_params_.brightness);
    process_params_.filtered = btFilter_.filter(process_params_.brightness);

    ++global_tick_;
    return IProcessing::state::WORKING;
}

} // namespace app
