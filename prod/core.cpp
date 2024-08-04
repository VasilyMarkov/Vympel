#include "core.hpp"
#include <numeric>
#include <qt6/QtCore/QThread>
#include <qt6/QtCore/QCoreApplication>

using namespace app;
using namespace constants;

Event::Event(params_t& params): params_(params) {}

Idle::Idle(params_t& params): Event(params)
{
    
}

void Idle::operator()(size_t tick)
{

}

Calibration::Calibration(params_t& params): Event(params)
{
    
}

void Calibration::operator()(size_t tick)
{

}

Measurement::Measurement(params_t& params): Event(params)
{
    
}

void Measurement::operator()(size_t)
{

}

Fsm::Fsm(params_t & params, size_t global_tick):
    params_(params), 
    global_tick_(global_tick),
    active_event_(std::make_unique<Idle>(params_)), 
    events_({
        {"idle", core_mode_t::IDLE},
        {"calibration", core_mode_t::CALIBRATION},
        {"meashurement", core_mode_t::MEASUREMENT},
    }){}

void app::Fsm::toggle(const QString& mode)
{
    if (mode_ == events_.at(mode)) return;

    mode_ = events_.at(mode);
    dispatchEvent();
}

void app::Fsm::callEvent()
{
    if (!active_event_) return;

    (*active_event_)(global_tick_);
}

void app::Fsm::dispatchEvent()
{
    active_event_.reset(nullptr);

    switch (mode_)
    {
    case core_mode_t::IDLE:
        active_event_ = std::make_unique<Idle>(params_);
    break;

    case core_mode_t::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(params_);
    break;

    case core_mode_t::MEASUREMENT:
        active_event_ = std::make_unique<Measurement>(params_);
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(params_);
    break;
    }
}


Core::Core(const std::string& filename): 
    capture_(filename), 
    filter_(filter::cutoff_frequency, filter::sample_rate), 
    fsm_(params_, global_tick_) 
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);
}

void Core::process()
{
    while(computerVision()) {
        fsm_.callEvent();
        
        emit sendData(params_);
        
        QThread::msleep(10);
        QCoreApplication::processEvents();

        ++global_tick_;
    }
    emit exit();
}

void app::Core::receiveData(const QString& mode)
{
    fsm_.toggle(mode);
}

bool app::Core::computerVision()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return false;
        
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);

        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        params_.filtered = filter_.Process(params_.brightness);
        
        return true;
        // cv::imshow("w", frame_);
}

