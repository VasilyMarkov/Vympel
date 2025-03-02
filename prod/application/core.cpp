#include <QThread>
#include <QCoreApplication>
#include "core.hpp"
#include "utility.hpp"
#include "logger.hpp"

namespace app
{
 
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule) {}

void Core::receiveTemperature(double temperature) noexcept
{
    temperature_ = temperature;
}

bool Core::process()
{
    static bool isLoggerCreated = false;

    while(process_unit_->process() != IProcessing::state::DONE) 
    {
        if(QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Core is closed";
            return true;
        }

        auto processParams = process_unit_->getProcessParams();
        json_["brightness"] = processParams.brightness;
        json_["filtered"] = processParams.filtered;
        json_["temperature"] = temperature_;
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);
        Q_EMIT sendData(QJsonDocument(json_));

        Q_EMIT requestTemperature();

        if(statement_ == CoreStatement::work) {
            if(!isLoggerCreated) {
                Logger::getInstance().createLog();
                isLoggerCreated = true;
            }
            onFSM();
            callEvent();
            global_data_.push_back(processParams.filtered);
            temperature_data_.push_back(temperature_);
        }
        else {
            offFSM();
            if(isLoggerCreated) {
                Logger::getInstance().log(global_data_, temperature_data_);
                global_data_.clear();
                isLoggerCreated = false;
            }
        }
        QThread::msleep(20);


        QCoreApplication::processEvents();
    }
    
    return true;
}

void Core::setBlEStatus() noexcept {
    bleIsReady_ = true;
}

void Core::receiveData(const QJsonDocument& json)
{

}

void Core::setCoreStatement(int state) noexcept {
    statement_ = static_cast<CoreStatement>(state);
}

void Core::receiveRateTemprature(double temperatureRate) noexcept {
    temperatureRate_ = temperatureRate;
}

std::shared_ptr<IProcessing> Core::getProcessUnit() const noexcept {
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
    case EventType::NO_STATE:
        return;
    break;    
    case EventType::IDLE:
        active_event_ = std::make_unique<Idle>(process_unit_, temperature_);
        Q_EMIT setRateTemprature(1);
    break;

    case EventType::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
    break;

    case EventType::MEASHUREMENT:
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Meashurement>(process_unit_);
        }
        Q_EMIT setRateTemprature(-1);
    break;

    case EventType::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_, temperature_);
        Q_EMIT setRateTemprature(1.5);
        std::cout << "COND POINT: " << process_unit_->getTick() << std::endl;
    break;

    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
        // emit runOptimizationProcess(std::vector<double>(std::next(std::begin(global_data_), 800), std::end(global_data_)));
        // emit runOptimizationProcess(std::vector<double>(std::begin(global_data_), std::end(global_data_)));
    break;
    
    default:
        return;
    break;
    }
}

void Core::onFSM()
{
    if(!isOnFSM) {
        toggle(EventType::IDLE);
        isOnFSM = true;
    }
}

void Core::offFSM()
{
    if(isOnFSM) {
        toggle(EventType::NO_STATE);
        isOnFSM = false;
    }
}

} //namespace app

