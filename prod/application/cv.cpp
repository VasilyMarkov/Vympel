#include <numeric>
#include "cv.hpp"
#include "configReader.hpp"
#include <filesystem>


namespace app
{
    
using namespace constants;
namespace fs = std::filesystem;

struct MappedBuffer {
    void *data;
    size_t size;
};

CameraProcessingModule::CameraProcessingModule()
{
    btFilter_.setup (ConfigReader::getInstance().get("parameters", "sampling_rate_filter_freq_Hz").toInt(), 
                     ConfigReader::getInstance().get("parameters", "filter_cutoff_freq_Hz").toInt());

	uint32_t num_cams = LibcameraApp::GetNumberCameras();

    uint32_t height = 480;
    uint32_t width = 640;

    frame_ = cv::Mat(height, width, CV_8UC3);

    cam_.options->video_width=width;
    cam_.options->video_height=height;
    cam_.options->framerate=30;
    cam_.options->verbose=true;

    cam_.startVideo();
}


//TODO Currently not working, lccv hassegfault inside.
IProcessing::state CameraProcessingModule::process()
{
    if (!cam_.getVideoFrame(frame_,1000)){
        return IProcessing::state::NODATA;
    }

    if(frame_.empty()) return IProcessing::state::NODATA;

    
    std::vector encode_params  = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", frame_, buffer_, encode_params);

    cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
    std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());

    process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
    
    process_params_.filtered = btFilter_.filter(process_params_.brightness);
    // qDebug() << process_params_.filtered;

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
    // receiveBuffer_.push({json["brightness"].toDouble(), json["valid"].toBool()});
    value_ = json["brightness"].toDouble();
}

size_t NetLogic::getValue()
{
    // if(receiveBuffer_.empty()) return std::nullopt;

    // auto value = receiveBuffer_.front();
    // receiveBuffer_.pop();
    return value_;
}

NetProcessModule::NetProcessModule(): filter_(filter::cutoff_frequency, filter::sample_rate) {
    btFilter_.setup (ConfigReader::getInstance().get("parameters", "sampling_rate_filter_freq_Hz").toInt(), 
        ConfigReader::getInstance().get("parameters", "filter_cutoff_freq_Hz").toInt());
    std::cout << "Py camera is active" << std::endl;
}

IProcessing::state NetProcessModule::process()
{   
    // if(auto net_value = netLogic.getValue(); net_value.has_value()) {
    //     if(!net_value.value().valid) return IProcessing::state::DONE;

        process_params_.brightness = netLogic.getValue(); 
        process_params_.filtered = btFilter_.filter(process_params_.brightness);

        return IProcessing::state::WORKING;
    // }
    // else {
    //     return IProcessing::state::NODATA;
    // }
    // ++global_tick_;
}

TestProcessModule::TestProcessModule():
    test_data_(
        readJsonLog(
            fs::path(
                fs::current_path().parent_path() / ConfigReader::getInstance().get("log_files", "log_folder").toString().toStdString()
            ),
            "log1"
        )
    ), 
    filter_(filter::cutoff_frequency, filter::sample_rate) {
        btFilter_.setup (ConfigReader::getInstance().get("parameters", "sampling_rate_filter_freq_Hz").toInt(), 
            ConfigReader::getInstance().get("parameters", "filter_cutoff_freq_Hz").toInt());

        auto sample_begin = ConfigReader::getInstance().get("parameters", "first_sample").toInt();

        test_data_ = decltype(test_data_)(std::begin(test_data_) + sample_begin, std::end(test_data_));
        std::cout << "Test Module is started" << std::endl;
    }

IProcessing::state TestProcessModule::process() 
{
    if(global_tick_ == test_data_.size()) return IProcessing::state::DONE;
    
    process_params_.brightness = test_data_[global_tick_].first;
    process_params_.filtered = btFilter_.filter(process_params_.brightness);
    process_params_.temperature = test_data_[global_tick_].second;

    ++global_tick_;
    return IProcessing::state::WORKING;
}

} // namespace app
