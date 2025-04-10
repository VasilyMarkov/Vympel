#include <QThread>
#include <QCoreApplication>
#include <QtConcurrent>
#include "core.hpp"
#include "utility.hpp"
#include "logger.hpp"
#include "cv.hpp"


namespace app
{
 
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule) 
{
    connect(&timer_, &QTimer::timeout, [this](){
        QtConcurrent::run([this](){
            Q_EMIT setRateTemprature(setRate);
        });
    });

    timer_.start(200);
}

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
        Q_EMIT sendCompressedImage(std::static_pointer_cast<CameraProcessingModule>(process_unit_)->getBuffer());
        // qDebug() << processParams.filtered;

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

void Core::receiveFitCoefficients(const std::vector<double>& coeffs)
{
    std::static_pointer_cast<End>(active_event_)->setCoeffs(coeffs);
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

    int start_point{};

    switch (mode_)
    {
    case EventType::NO_STATE:
        return;
    break;    
    case EventType::IDLE:
        active_event_ = std::make_unique<Idle>(process_unit_, temperature_);
        // Q_EMIT setRateTemprature(1.7);
        setRate = 1.7;
    break;

    case EventType::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
    break;

    case EventType::MEASHUREMENT:
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Meashurement>(process_unit_);
        }
        // Q_EMIT setRateTemprature(-1.7);
        
        setRate = -1.7;
    break;

    case EventType::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_, temperature_);
        setRate = 1.7;
        // Q_EMIT setRateTemprature(1.7);
        start_mark_ = process_unit_->getTick();
        std::cout << "COND POINT: " << start_mark_ << std::endl;
    break;

    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
        // emit runOptimizationProcess(std::vector<double>(std::next(std::begin(global_data_), 800), std::end(global_data_)));
        Q_EMIT runOptimizationProcess(std::vector<double>(std::begin(global_data_) + start_mark_, std::end(global_data_)));
        end_mark_ = process_unit_->getTick();
        std::cout << "END POINT: " << end_mark_ << std::endl;
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

