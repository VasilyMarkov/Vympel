#include <numeric>
#include "cv.hpp"

using namespace cv;

// app::Camera::Camera():
//     camera_manager_(std::make_unique<libcamera::CameraManager>())
// {   
//     using namespace libcamera;

//     camera_manager_->start();

//     for (auto&& camera : camera_manager_->cameras()) std::cout << camera->id() << std::endl;
        
//     auto cameras = camera_manager_->cameras();
//     if (cameras.empty()) {
//         camera_manager_->stop();
//         throw std::runtime_error("No cameras were identified on the system.");
//     }
//     auto cameraId = camera_manager_->cameras()[0]->id();
//     auto camera = camera_manager_->get(cameraId);
//     if (!camera_) throw std::runtime_error("Failed to find camera " + cameraId);
//     camera_->acquire();

//     auto config = camera_->generateConfiguration({StreamRole::Viewfinder});

//     StreamConfiguration &streamConfig = config->at(0);
//     std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

//     streamConfig.size.width = 640;
//     streamConfig.size.height = 480;

//     config->validate();
//     std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;

//     if (!camera_->configure(config.get())) throw std::runtime_error("Failed to configure camera");

    // FrameBufferAllocator *allocator = new FrameBufferAllocator(camera_);

    // for (StreamConfiguration &cfg : *config) {
    //     int ret = allocator->allocate(cfg.stream());
    //     if (ret < 0) {
    //         std::cerr << "Can't allocate buffers" << std::endl;
    //         // return -ENOMEM;
    //     }

    //     size_t allocated = allocator->buffers(cfg.stream()).size();
    //     std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
    // }
    // std::cout << "release" << std::endl;

//     camera_->release();
//     camera_manager_->stop();
// }


static LibCamera cam;
static uint32_t width = 640;
static uint32_t height = 480;
static uint32_t stride = 0;
static char key;
static int window_width = 640;
static int window_height = 480;
static LibcameraOutData frameData;
static ControlList controls_;

app::CVision::CVision(const std::string& filename): 
    // capture_(0), 
    filter_(app::constants::filter::cutoff_frequency, app::constants::filter::sample_rate)
{

    // if(!capture_.isOpened()) throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);

    if (width > window_width)
    {
        cv::namedWindow("libcamera-demo", cv::WINDOW_NORMAL);
        cv::resizeWindow("libcamera-demo", window_width, window_height);
    } 

    int ret = cam.initCamera();
    cam.configureStill(width, height, formats::RGB888, 1, 0);
    
    int64_t frame_time = 1000000 / 60;
    controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    cam.set(controls_);
    
    cam.startCamera();
    cam.VideoStream(&width, &height, &stride);
}

bool app::CVision::process()
{

    // capture_ >> frame_;
    
    // if(frame_.empty()) return false;
    // // cv::imshow("w", frame_);
    // cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
    
    // std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
    // process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
    
    // process_params_.filtered = filter_.Process(process_params_.brightness);

    // ++global_tick_;
    std::cout << "process" << std::endl;
    while (true) {
        if (!cam.readFrame(&frameData))
            continue;
        Mat im(height, width, CV_8UC3, frameData.imageData, stride);

        imshow("libcamera-demo", im);
        key = waitKey(1);

        cam.returnFrameBuffer(frameData);
    }

    return true;
        
}

app::CVision::~CVision() {
    destroyAllWindows();
    cam.stopCamera();
    cam.closeCamera();
}