#include <QThread>
#include <QCoreApplication>
#include "core.hpp"
#include "utility.hpp"

namespace app
{
    
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule),
    fsm_(std::make_unique<Fsm>(process_unit_, [this](){std::cout << "callback" << std::endl; Q_EMIT requestTemperature();})),
    events_({
        {"idle", core_mode_t::IDLE},
        {"calibration", core_mode_t::CALIBRATION},
        {"meashurement", core_mode_t::MEASUREMENT},
    }){}


void app::Core::receiveTemperature(double temperature) const
{
    std::cout << temperature << std::endl;
}

bool Core::process()
{
    while(process_unit_->process()) {
        fsm_->callEvent();
        
        Q_EMIT sendData(process_unit_->getProcessParams());
        QThread::msleep(20);
        QCoreApplication::processEvents();
    }
    Q_EMIT exit();

    return true;
}

void app::Core::bleDeviceConnected()
{
    
}

void app::Core::receiveData(const QString& mode)
{
    // fsm_->toggle(events_.at(mode)); TODO out_of_range exception
}

std::shared_ptr<IProcessing> Core::getProcessUnit() const {
    return process_unit_;
}

} //namespace app

