#include <QThread>
#include <QCoreApplication>
#include "core.hpp"
#include "utility.hpp"

namespace app
{
 
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule),
    active_event_(std::make_unique<Idle>(process_unit_)),
    events_({
        {"idle", core_mode_t::IDLE},
        {"calibration", core_mode_t::CALIBRATION},
        {"meashurement", core_mode_t::MEASUREMENT},
    }){}


void Core::receiveTemperature(double temperature) const
{
    std::cout << temperature << std::endl;
}

bool Core::process()
{
    while(process_unit_->process()) {
        callEvent();
        
        Q_EMIT sendData(process_unit_->getProcessParams());
        QThread::msleep(20);
        QCoreApplication::processEvents();
    }
    Q_EMIT exit();

    return true;
}

void Core::bleDeviceConnected()
{
    
}

void Core::receiveData(const QString& mode)
{
    // fsm_->toggle(events_.at(mode)); TODO out_of_range exception
}

std::shared_ptr<IProcessing> Core::getProcessUnit() const {
    return process_unit_;
}

void Core::toggle(core_mode_t mode)
{
    if (mode_ == mode) return;

    mode_ = mode;
    dispatchEvent();
}

void Core::callEvent()
{
    if (!active_event_) return;
    
    auto event_result = (*active_event_)();

    if (event_result != std::nullopt) {
        toggle(event_result.value());
    }
}

void Core::dispatchEvent()
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
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Measurement>(process_unit_);
        }
    break;
    case core_mode_t::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_);
        Q_EMIT requestTemperature();
    break;
    case core_mode_t::END:
        active_event_ = std::make_unique<End>(process_unit_);
        Q_EMIT requestTemperature();
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

} //namespace app

