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
    static bool isLoggerCreated = false;

    while(process_unit_->process() != IProcessing::state::DONE) {
        if(bleIsReady_) {
            Q_EMIT requestTemperature();
            // std::cout << temperature_ << std::endl;
            
        }
        if(statement_ == CoreStatement::HALT) {
            offFSM();
            if(isLoggerCreated) {
                // Logger::getInstance().log(global_data_, temperature_data_);
                isLoggerCreated = false;
            }
        }
        else {
            if(!isLoggerCreated) {
                // Q_EMIT requestFastHeating();
                // Logger::getInstance().createLog();
                static bool cool = true;
                static bool heat = true;
                std::cout << (int)h_statement_ << std::endl;
                if(h_statement_ == HygrovisionStatement::FAST_COOLING) {
                    if(cool) {
                        std::cout << "cooling" << std::endl;
                        Q_EMIT requestFastCooling();
                        cool = false;
                        heat = true;
                    }
                }
                else if(h_statement_ == HygrovisionStatement::FAST_HEATING) {
                    if(heat) {      
                        std::cout << "heating" << std::endl;   
                        Q_EMIT requestFastHeating();
                        heat = false;
                        cool = true;
                    }
                }
                isLoggerCreated = true;
            }
            onFSM();
            callEvent();
            auto processParams = process_unit_->getProcessParams();
            json_["brightness"] = processParams.brightness;
            json_["filtered"] = processParams.filtered;
            std::cout << processParams.filtered << std::endl;
            json_["temperature"] = temperature_;
            global_data_.push_back(processParams.filtered);
            temperature_data_.push_back(temperature_);
        }
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);
        Q_EMIT sendData(QJsonDocument(json_));
        QThread::msleep(20); //TODO Need to implement via timer
        QCoreApplication::processEvents();
    }
    // Q_EMIT exit();

    return true;
}

void Core::changeHygroVisionStatement(HygrovisionStatement newStatement) {
    if(h_statement_ != newStatement) {
        h_statement_ == newStatement;
    }
    return;
}

void Core::callOnce(CoreStatement) {
    
}

void Core::setBlEStatus() {
    bleIsReady_ = true;
}

void Core::receiveData(const QJsonDocument& json)
{
    statement_ = static_cast<CoreStatement>(json["statement"].toInt());
    h_statement_ = static_cast<HygrovisionStatement>(json["h_statement"].toInt());
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
    break;

    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
    break;
    
    default:
        active_event_ = std::make_unique<Idle>(process_unit_);
    break;
    }
}

void Core::onFSM()
{
    if(!isOnFSM) {
        toggle(EventType::CALIBRATION);
        isOnFSM = true;
    }
}

void Core::offFSM()
{
    if(isOnFSM) {
        toggle(EventType::IDLE);
        isOnFSM = false;
    }
}

} //namespace app

