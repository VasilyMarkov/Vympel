#include "core.hpp"
#include <numeric>
#include <qt6/QtCore/QThread>
#include <qt6/QtCore/QCoreApplication>

using namespace app;
using namespace constants;

Event::Event(std::weak_ptr<CVision> cv):cv_(cv), start_tick_(cv_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<CVision> cv): Event(cv)
{
    std::cout << "idle " << std::endl;
}

app::Idle::~Idle()
{
    std::cout << "~idle" << std::endl;
}

std::optional<core_mode_t> Idle::operator()()
{
    if(cv_.lock()->getTick() - start_tick_ >= WAITING_TICKS) {
        return core_mode_t::CALIBRATION;   
    }
    return std::nullopt;
}

Calibration::Calibration(std::weak_ptr<CVision> cv): Event(cv)
{
    std::cout << "calib" << std::endl;
    data_.reserve(buffer::CALIB_SIZE);
}

app::Calibration::~Calibration()
{
    std::cout << "~calib" << std::endl;
}

std::optional<core_mode_t> Calibration::operator()()
{
    if(cv_.lock()->getTick() - start_tick_ >= buffer::CALIB_SIZE) 
    {
        cv_.lock()->getCalcParams().mean_filtered = std::accumulate(std::begin(data_), std::end(data_), 0)/data_.size();
        cv_.lock()->getCalcParams().event_completeness.calibration = true;
        return core_mode_t::MEASUREMENT;    
    }
    data_.push_back(cv_.lock()->getCvParams().filtered);

    return std::nullopt;
}

Measurement::Measurement(std::weak_ptr<CVision> cv): Event(cv)
{
    std::cout << "measure" << std::endl;
}

app::Measurement::~Measurement()
{
    std::cout << "~measure" << std::endl;
}

std::optional<core_mode_t> Measurement::operator()()
{
    return std::nullopt;
}

Fsm::Fsm(std::weak_ptr<CVision> cv): cv_(cv), active_event_(std::make_unique<Idle>(cv_)) {}

void app::Fsm::toggle(core_mode_t mode)
{
    if (mode_ == mode) return;

    mode_ = mode;
    dispatchEvent();
}

void app::Fsm::callEvent()
{
    if (!active_event_) return;
    
    auto event_result = (*active_event_)();

    if (event_result != std::nullopt) {
        toggle(event_result.value());
    }
}

void app::Fsm::dispatchEvent()
{
    active_event_.reset(nullptr);

    switch (mode_)
    {
    case core_mode_t::IDLE:
        active_event_ = std::make_unique<Idle>(cv_);
    break;

    case core_mode_t::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(cv_);
    break;

    case core_mode_t::MEASUREMENT:
        if(cv_.lock()->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Measurement>(cv_);
        }
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(cv_);
    break;
    }
}

Core::Core(const std::string& filename): 
    cv_(std::make_shared<CVision>(filename)),
    fsm_(std::make_unique<Fsm>(cv_)),
    events_({
        {"idle", core_mode_t::IDLE},
        {"calibration", core_mode_t::CALIBRATION},
        {"meashurement", core_mode_t::MEASUREMENT},
    }){}

app::CVision::CVision(const std::string& filename):capture_(filename), filter_(filter::cutoff_frequency, filter::sample_rate)
{
    if(!capture_.isOpened())
        throw std::runtime_error("file open error");

    // cv::namedWindow( "w", 1);
}

size_t app::CVision::getTick() const noexcept
{
    return global_tick_;
}

cv_params_t app::CVision::getCvParams() const noexcept
{
    return cv_params_;
}

calc_params_t& app::CVision::getCalcParams() noexcept
{
    return calc_params_;
}

bool Core::process()
{
    while(cv_->process()) {
        fsm_->callEvent();
        
        emit sendData(cv_->getCvParams());
        QThread::msleep(10);
        QCoreApplication::processEvents();
    }
    emit exit();

    return true;
}

void app::Core::receiveData(const QString& mode)
{
    fsm_->toggle(events_.at(mode));
}

bool app::CVision::process()
{
        capture_ >> frame_;
        
        if(frame_.empty()) return false;
        
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);

        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        cv_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        cv_params_.filtered = filter_.Process(cv_params_.brightness);

        if(calc_params_.event_completeness.calibration) {
            cv_params_.filtered -= calc_params_.mean_filtered;
            cv_params_.brightness -= calc_params_.mean_filtered;
        }

        ++global_tick_;
        // cv::imshow("w", frame_);
        return true;
        
}
