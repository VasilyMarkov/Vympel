#include "core.hpp"
#include <numeric>

using namespace app;
using namespace constants;

Core::Core(const std::string& filename): capture_(filename), filter_(filter::cutoff_frequency, filter::sample_rate)
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");

    cv::namedWindow( "w", 1);
}

void Core::process()
{
    while(true) {
        capture_ >> frame_;

        if(frame_.empty()) return;
        
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);

        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        params_.filtered = filter_.Process(params_.brightness);

        notify();

        cv::imshow("w", frame_);
        cv::waitKey(20); // waits to display frame
    }
    cv::waitKey(0); // key press to close window
}

void Core::attach(std::unique_ptr<IObserver> observer)
{
    observers_.push_back(std::move(observer));
}

void Core::detach(std::unique_ptr<IObserver> observer)
{
    if (observers_.empty()) return;

    observers_.remove(observer);
}

void Core::notify() const
{
    if(observers_.empty()) return;

    for(auto&& obs : observers_) {
        obs->update(params_);
    }
}
