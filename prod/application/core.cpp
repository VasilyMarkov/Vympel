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
        {"idle", EventType::IDLE},
        {"calibration", EventType::CALIBRATION},
        {"meashurement", EventType::MEASHUREMENT},
    }){}


void Core::receiveTemperature(double temperature) const
{
    std::cout << temperature << std::endl;
}

bool Core::process()
{
    while(process_unit_->process()) {
        callEvent();
        if(statement_ == CoreStatement::HALT) {
            offFSM();
        }
        else {
            onFSM();
            auto processParams = process_unit_->getProcessParams();
            json_["brightness"] = processParams.brightness;
            json_["filtered"] = processParams.filtered;
            
        }
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);
        Q_EMIT sendData(QJsonDocument(json_));
        QThread::msleep(20); //TODO Need to implement via timer
        QCoreApplication::processEvents();
    }
    Q_EMIT exit();

    return true;
}

void Core::bleDeviceConnected()
{
    
}

void Core::receiveData(const QJsonDocument& json)
{
    statement_ = static_cast<CoreStatement>(json["statement"].toInt());
}

std::shared_ptr<IProcessing> Core::getProcessUnit() const {
    return process_unit_;
}

void Core::toggle(EventType mode)
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
    case EventType::IDLE:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;

    case EventType::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
    break;

    case EventType::MEASHUREMENT:
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<MEASHUREMENT>(process_unit_);
        }
    break;
    case EventType::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_);
        Q_EMIT requestTemperature();
    break;
    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
        Q_EMIT requestTemperature();
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

void Core::onFSM()
{
    if(!isOnFSM) {
        mode_ = EventType::CALIBRATION;
        isOnFSM = true;
    }
}

void Core::offFSM()
{
    if(isOnFSM) {
        mode_ = EventType::IDLE;
        isOnFSM = false;
    }
}

} //namespace app

