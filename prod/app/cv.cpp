#include <numeric>
#include "cv.hpp"

app::Camera::Camera():
    camera_(std::make_shared<libcamera::Camera>())
    camera_manager_(std::make_unique<libcamera::CameraManager>()), 
    config_(std::make_unique<libcamera::CameraConfiguration>(camera_->generateConfiguration( { StreamRole::Viewfinder } )))
{   
    using namespace libcamera;

    camera_manager_->start();

    for (auto&& camera : camera_manager_->cameras()) std::cout << camera->id() << std::endl;
        
    auto cameras = camera_manager_->cameras();
    if (cameras.empty()) {
        camera_manager_->stop();
        throw std::runtime_error("No cameras were identified on the system.");
    }

    auto cameraId = cameras[0]->id();
    auto camera = camera_manager_->get(cameraId);

}


app::CVision::CVision(const std::string& filename): 
    capture_(0), 
    filter_(app::constants::filter::cutoff_frequency, app::constants::filter::sample_rate)
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);
}

bool app::CVision::process()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return false;
        // cv::imshow("w", frame_);
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
        
        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        process_params_.filtered = filter_.Process(process_params_.brightness);

        ++global_tick_;
        
        return true;
        
}

