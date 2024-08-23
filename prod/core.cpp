#include <numeric>
#include <qt6/QtCore/QThread>
#include <qt6/QtCore/QCoreApplication>
#include "core.hpp"
#include "utility.hpp"

using namespace app;
using namespace constants;

Event::Event(std::weak_ptr<IProcessing> cv):cv_(cv), start_tick_(cv_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv): Event(cv)
{
    
}

std::optional<core_mode_t> Idle::operator()()
{
    if(cv_.lock()->getTick() - start_tick_ >= WAITING_TICKS) {
        return core_mode_t::CALIBRATION;   
    }
    return std::nullopt;
}

Calibration::Calibration(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "calib" << std::endl;
    data_.reserve(buffer::CALIB_SIZE);
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

Measurement::Measurement(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "measure" << std::endl;
    data_.reserve(buffer::MEASUR_SIZE);
    least_square_samples_.reserve(100);
}

std::optional<core_mode_t> Measurement::operator()()
{
    if(cv_.lock()->getTick() - start_tick_ >= buffer::MEASUR_SIZE) 
    {

    }

    // if(local_tick_ % SAMPLE_FREQ == 0) {
    //     if(least_square_samples_.size() == 10) {
    //         if(coeffs_.size() == 1) {
    //             coeffs_.push_back(findLineCoeff());
    //             auto difference = coeffs_[1]-coeffs_[0];   
    //             std::cout << difference << std::endl; 
    //         }
    //         coeffs_.push_back(findLineCoeff());
    //         least_square_samples_.resize(0);
    //     }
    //     least_square_samples_.push_back(cv_.lock()->getCvParams().filtered);
    // }

    if(least_square_samples_.size() == least_square_samples_.capacity()) {
        // std::cout << findLineCoeff() << std::endl;
        
        if(coeffs_.size() == 2) {
            std::cout << coeffs_[1]/coeffs_[0] << std::endl;
            coeffs_.pop_front();
        }
        coeffs_.push_back(findLineCoeff());
        least_square_samples_.clear();
    }

    least_square_samples_.push_back(cv_.lock()->getCvParams().filtered);


    data_.push_back(cv_.lock()->getCvParams().filtered);

    ++local_tick_;
    return std::nullopt;
}

/* @brief Least squares
*
* Function calculates gradient of line approximation of data
*
* Input data takes from least_square_samples_
*
* @return linear coefficient a from line equation y = ax+b
*/
double app::Measurement::findLineCoeff() 
{
    auto& y = least_square_samples_;
    auto n = y.size();
    std::vector<double> x(n);
    std::iota(std::begin(x), std::end(x), 0);

    // auto x_sum = std::accumulate(std::begin(x), std::end(x), 0);
    // auto y_sum = std::accumulate(std::begin(y), std::end(y), 0);
    // auto x2_sum = std::inner_product(std::begin(x), std::end(x), std::begin(x), 0);
    // auto xy_sum = std::inner_product(std::begin(x), std::end(x), std::begin(y), 0);

    // auto nominator = n*xy_sum-x_sum*y_sum;
    // auto denominator = n*x2_sum-x_sum*x_sum;
    
    // std::cout << nominator/denominator << std::endl;
    // if (auto epsilon{1e-7}; std::fabs(denominator - epsilon) <= 0) throw std::runtime_error("Divide by zero in least squares");

    // return nominator/denominator; 
    double x_sum{0}, y_sum{0}, x2_sum{0}, xy_sum{0};
    for (auto i = 0; i < y.size(); ++i)
    {
        x_sum+=x[i];
        y_sum+=y[i];
        x2_sum+=std::pow(x[i],2);
        xy_sum+=x[i]*y[i];
    }
    double a=(n*xy_sum-x_sum*y_sum)/(n*x2_sum-x_sum*x_sum);
    double b=(x2_sum*y_sum-x_sum*xy_sum)/(x2_sum*n-x_sum*x_sum);
    return a;
}

Fsm::Fsm(std::weak_ptr<IProcessing> cv): cv_(cv), active_event_(std::make_unique<Idle>(cv_)) {}

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

    cv::namedWindow( "w", 1);
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
        cv::imshow("w", frame_);
        cv::cvtColor(frame_, frame_, cv::COLOR_BGR2GRAY, 0);
        
        std::vector<uint8_t> v(frame_.begin<uint8_t>(), frame_.end<uint8_t>());
        cv_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);
        
        cv_params_.filtered = filter_.Process(cv_params_.brightness);

        if(calc_params_.event_completeness.calibration) {
            cv_params_.filtered -= calc_params_.mean_filtered;
            cv_params_.brightness -= calc_params_.mean_filtered;
        }

        ++global_tick_;
        
        return true;
        
}
