#include <numeric>
#include "cv.hpp"

using namespace app;
using namespace constants;

CVision::CVision(const std::string& filename):capture_(filename), filter_(filter::cutoff_frequency, filter::sample_rate)
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");

    cv::namedWindow( "w", 1);
}

bool CVision::process()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return false;
        cv::imshow("w", frame_);
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
        
        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        process_params_.filtered = filter_.Process(process_params_.brightness);

        // if(calc_params_.event_completeness.calibration) {
        //     process_params_.filtered -= calc_params_.mean_filtered;
        //     process_params_.brightness -= calc_params_.mean_filtered;
        // }

        ++global_tick_;
        
        return true;
        
}
