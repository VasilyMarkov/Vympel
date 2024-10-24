#include "fsm.hpp"

namespace app
{

// template<typename Event>
// Fsm::Fsm(eventFabric<Event>){} 

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
        request_temperature_callback_();
    break;
    case core_mode_t::END:
        active_event_ = std::make_unique<End>(process_unit_);
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

} //namespace app