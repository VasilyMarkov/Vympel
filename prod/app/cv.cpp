#include <numeric>
#include "cv.hpp"


app::CVision::CVision(const std::string& filename): 
    capture_(0), 
    filter_(app::constants::filter::cutoff_frequency, app::constants::filter::sample_rate)
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);

    using namespace libcamera;

    // auto camera = std::make_shared<Camera>();
    auto cm = std::make_unique<CameraManager>();
	cm->start();
    for (auto&& camera : cm->cameras())
        std::cout << camera->id() << std::endl;
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
