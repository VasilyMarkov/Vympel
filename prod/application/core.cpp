#include <QThread>
#include <QCoreApplication>
#include "core.hpp"
#include "utility.hpp"
#include "logger.hpp"

namespace app
{
 
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule),
    active_event_(std::make_unique<Idle>(process_unit_)) {}

void Core::receiveTemperature(double temperature) noexcept
{
    temperature_ = temperature;
}

bool Core::process()
{
    while(process_unit_->process() != IProcessing::state::DONE) {
        if(statement_ == CoreStatement::HALT) {
            offFSM();
        }
        else {
            onFSM();
            callEvent();
            auto processParams = process_unit_->getProcessParams();
            json_["brightness"] = processParams.brightness;
            json_["filtered"] = processParams.filtered;
            global_data_.push_back(processParams.filtered);
        }
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);
        json_["temperature"] = static_cast<int>(statement_);
        Q_EMIT sendData(QJsonDocument(json_));
        QThread::msleep(20); //TODO Need to implement via timer
        QCoreApplication::processEvents();
    }
    // logger.log(global_data_);
    // Q_EMIT exit();

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
        global_data_.clear();
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;

    case EventType::CALIBRATION:
        // logger.createLog();
        active_event_ = std::make_unique<Calibration>(process_unit_);
    break;

    case EventType::MEASHUREMENT:
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Meashurement>(process_unit_);
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

