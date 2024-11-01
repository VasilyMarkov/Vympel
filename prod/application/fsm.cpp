#include "fsm.hpp"

namespace app
{

Fsm::Fsm(std::weak_ptr<IProcessing> cv, const std::function<void()>& callback): 
    process_unit_(cv), 
    active_event_(std::make_unique<Idle>(process_unit_)),
    request_temperature_callback_(callback) {}

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
        std::cout << "Temp: " << temp_ << std::endl;
    break;

    case core_mode_t::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
        std::cout << "Temp: " << temp_ << std::endl;
    break;

    case core_mode_t::MEASUREMENT:
        if(process_unit_.lock()->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Measurement>(process_unit_);
        }
        // print(temperature_);
    break;
    case core_mode_t::CONDENSATION:
        if(c_temp_ > 20) mode_ = core_mode_t::MEASUREMENT;
        else {
            active_event_ = std::make_unique<Сondensation>(process_unit_);
            std::cout << "Condesation temperature: " << temp_ << std::endl;
            c_temp_ = temp_;
        }
    break;
    case core_mode_t::END: 

        active_event_ = std::make_unique<End>(process_unit_);
        
        // print(temperature_);
        // std::cout << "Tick: " <<  active_event_->v_tick << std::endl;
        std::cout << "Vaporization temperature: " << temperature_.lower_bound(active_event_->v_tick)->second << std::endl;
        std::cout << "Half sum: " 
            << (temperature_.lower_bound(active_event_->v_tick)->second + c_temp_)/2 << std::endl;
        temperature_.clear();
    break;
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

void Fsm::setTemp(double temp) {
    temp_ = temp;
    temperature_.emplace(process_unit_.lock()->getTick(), temp);
    // std::cout << temp << std::endl;
}

} //namespace app