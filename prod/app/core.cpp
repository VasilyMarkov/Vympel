#include <qt6/QtCore/QThread>
#include <qt6/QtCore/QCoreApplication>
#include "core.hpp"
#include "utility.hpp"

using namespace app;
using namespace constants;

Event::Event(std::weak_ptr<IProcessing> cv):process_unit_(cv), start_tick_(process_unit_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv): Event(cv)
{
    
}

std::optional<core_mode_t> Idle::operator()()
{
    if(process_unit_.lock()->getTick() - start_tick_ >= WAITING_TICKS) {
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
    if(process_unit_.lock()->getTick() - start_tick_ >= buffer::CALIB_SIZE) 
    {
        // process_unit_.lock()->getCalcParams().mean_filtered = std::accumulate(std::begin(data_), std::end(data_), 0)/data_.size();
        process_unit_.lock()->getCalcParams().event_completeness.calibration = true;
        auto [mean, std_deviation] = meanVar(data_);
        process_unit_.lock()->getCalcParams().mean_filtered = mean;
        std::cout << mean << ' ' << std_deviation << std::endl;
        return core_mode_t::MEASUREMENT;    
    }
    data_.push_back(process_unit_.lock()->getProcessParams().filtered);

    return std::nullopt;
}

Measurement::Measurement(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "measure" << std::endl;
    data_.reserve(buffer::MEASUR_SIZE);
    mean_data.reserve(100);
    coeffs.resize(5);
}

bool isPositiveGrowth(double coeff) {
    static double local_coeff = 0;
    auto greater = coeff > local_coeff;
    local_coeff = coeff;
    return greater;
}

std::optional<core_mode_t> Measurement::operator()()
{
    if(mean_data.size() == mean_data.capacity()) {
        auto mean = std::accumulate(std::begin(mean_data), std::end(mean_data), 0.0)/mean_data.size();

        coeffs.pop_front();
        coeffs.push_back(isPositiveGrowth(mean));

        auto cnt = std::count_if(std::begin(coeffs), std::end(coeffs), [](bool val){return val == true;});
        std::cout << "tick: " << process_unit_.lock()->getTick() << ' ' << cnt << std::endl;
        std::cout << mean << std::endl;

        if(cnt > 3) {
            std::cout << "START" << ' ' << process_unit_.lock()->getTick() << std::endl;
            return core_mode_t::CONDENSATION;   
        }

        mean_data.clear();
    }

    mean_data.push_back(process_unit_.lock()->getProcessParams().filtered);


    ++local_tick_;
    return std::nullopt;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> process_unit): Event(process_unit)
{
    std::cout << "condensation" << std::endl;
}

std::optional<core_mode_t> app::Сondensation::operator()()
{
    return std::nullopt;
}


Fsm::Fsm(std::weak_ptr<IProcessing> cv): process_unit_(cv), active_event_(std::make_unique<Idle>(process_unit_)) {}

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
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;

    case core_mode_t::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
    break;

    case core_mode_t::MEASUREMENT:
        if(process_unit_.lock()->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Measurement>(process_unit_);
        }
    break;
    case core_mode_t::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_);
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule),
    fsm_(std::make_unique<Fsm>(process_unit_)),
    events_({
        {"idle", core_mode_t::IDLE},
        {"calibration", core_mode_t::CALIBRATION},
        {"meashurement", core_mode_t::MEASUREMENT},
    }){}


bool Core::process()
{
    while(process_unit_->process()) {
        fsm_->callEvent();
        
        emit sendData(process_unit_->getProcessParams());
        QThread::msleep(20);
        QCoreApplication::processEvents();
    }
    emit exit();

    return true;
}

void app::Core::receiveData(const QString& mode)
{
    fsm_->toggle(events_.at(mode));
}

std::shared_ptr<IProcessing> Core::getProcessUnit() const {
    return process_unit_;
}

