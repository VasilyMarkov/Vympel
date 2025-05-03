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

	uint32_t num_cams = LibcameraApp::GetNumberCameras();

    uint32_t height = 480;
    uint32_t width = 640;

    frame_ = cv::Mat(height, width, CV_8UC3);

    cam_.options->video_width=width;
    cam_.options->video_height=height;
    cam_.options->framerate=30;
    cam_.options->verbose=true;
    // cv::namedWindow("Video",cv::WINDOW_NORMAL);
    cam_.startVideo();

    // int ret = cam.initCamera();
    // cam.configureStill(width, height, formats::RGB888, 1, 0);
    
    // int64_t frame_time = 1000000 / 60;
    // controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    // cam.set(controls_);
    
    // cam.startCamera();
    // cam.VideoStream(&width, &height, &stride);

    std::cout << "Camera Module is started" << std::endl;
}

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
        
        process_params_.filtered = filter_.filter(process_params_.brightness);
        // qDebug() << process_params_.filtered;

        ++global_tick_;
        
        return IProcessing::state::WORKING;
        
}

static std::shared_ptr<Camera> camera;
static std::vector<MappedBuffer> mappedBuffers;


static void requestComplete(Request *request)
{
    // if (request->status() == Request::RequestCancelled)
    //     return;

    // const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();

    // for (auto bufferPair : buffers) {
    //     FrameBuffer *buffer = bufferPair.second;
    //     const FrameMetadata &metadata = buffer->metadata();
    // }

    // std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << " bytesused: ";

    // unsigned int nplane = 0;
    // for (const FrameMetadata::Plane &plane : metadata.planes())
    // {
    //     std::cout << plane.bytesused;
    //     if (++nplane < metadata.planes().size()) std::cout << "/";
    // }

    // std::cout << std::endl;

    // request->reuse(Request::ReuseBuffers);
    // camera->queueRequest(request);

    if (request->status() == Request::RequestComplete) {
        // size_t bufferIndex = request->cookie();
        // void *frameData = mappedBuffers[bufferIndex].data;
        // double* ready_data = static_cast<double*>(frameData);
        // size_t size = mappedBuffers[bufferIndex].size;
        // std::vector<double> frame_data;
        // frame_data.reserve(size);
        // for(auto i = 0 ; i < size; ++i) {
        //     frame_data.push_back(ready_data[i]);
        // }
        // std::cout << frame_data[0] << std::endl;
        // Process frame data here (e.g., save to file)
        // std::ofstream outFile("frame.raw", std::ios::binary | std::ios::app);
        // outFile.write(static_cast<char*>(frameData), size);
        // outFile.close();

        // Re-queue the request
        std::cout << "req" << std::endl;
        request->reuse();
        camera->queueRequest(request);
    }
}

TestCameraProcessingModule::TestCameraProcessingModule():
    camera_manager_(std::make_unique<CameraManager>())
{
    camera_manager_->start();
    for (auto const &camera : camera_manager_->cameras())
        std::cout << camera->id() << std::endl;

    auto cameras = camera_manager_->cameras();
    if (cameras.empty()) {
        std::cout << "No cameras were identified on the system." << std::endl;
        camera_manager_->stop();
    }

    std::string cameraId = cameras[0]->id();

    camera = camera_manager_->get(cameraId);
    camera->acquire();

    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration( { StreamRole::Viewfinder } );
    StreamConfiguration &streamConfig = config->at(0);
    std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;
    streamConfig.size.width = 640;
    streamConfig.size.height = 480;
    config->validate();
    std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;
    camera->configure(config.get());

    auto allocator = std::make_unique<FrameBufferAllocator>(camera);
    allocator->allocate(streamConfig.stream());
    const auto &buffers = allocator->buffers(streamConfig.stream());
    // for (StreamConfiguration &cfg : *config) {
    //     int ret = allocator->allocate(cfg.stream());
    //     if (ret < 0) {
    //         std::cerr << "Can't allocate buffers" << std::endl;
    //     }

    //     size_t allocated = allocator->buffers(cfg.stream()).size();
    //     std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
    // }

    for (const auto &buffer : buffers) {
        const FrameBuffer::Plane &plane = buffer->planes().front();
        void *data = mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);
        mappedBuffers.push_back({data, plane.length});
    }

    Stream *stream = streamConfig.stream();
    std::vector<std::unique_ptr<Request>> requests;

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<Request> request = camera->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request" << std::endl;
        }
    
        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                  << std::endl;
        }
    
        requests.push_back(std::move(request));
    }

    camera->requestCompleted.connect(requestComplete);

    camera->start();
    for (std::unique_ptr<Request> &request : requests) {
        camera->queueRequest(request.get());
    }
}

IProcessing::state TestCameraProcessingModule::process()
{   
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
