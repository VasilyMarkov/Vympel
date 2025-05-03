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
    // connect(&timer_, &QTimer::timeout, [this](){
    //     QtConcurrent::run([this](){
    //         Q_EMIT setRateTemprature(setRate);
    //     });
    // });

    connect(&timer_, &QTimer::timeout, this, &Core::process);
    connect(&timer_, &QTimer::timeout, [this](){
        static size_t cnt = 0;
        if(cnt % 10 == 0) {
            Q_EMIT setRateTemprature(setRate);
            cnt = 0;
        }
        ++cnt;
    });

    timer_.start(25);
}

void Core::receiveTemperature(double temperature) noexcept
{
    temperature_ = temperature;
}

bool Core::process()
{
    static bool isLoggerCreated = false;

    if(QThread::currentThread()->isInterruptionRequested()) {
        qDebug() << "Core is closed";
        return true;
    }

    process_unit_->process();
    // Q_EMIT sendCompressedImage(std::static_pointer_cast<CameraProcessingModule>(process_unit_)->getBuffer());
    

    if(statement_ == CoreStatement::work) {
        if(!isLoggerCreated) {
            Logger::getInstance().createLog();
            isLoggerCreated = true;
        }

        auto processParams = process_unit_->getProcessParams();
        json_["brightness"] = processParams.brightness;
        json_["filtered"] = processParams.filtered;
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);

        if(ConfigReader::getInstance().isBleEnable()) {
            json_["temperature"] = temperature_;
        }
        else {
            json_["temperature"] = processParams.temperature;
        }

        Q_EMIT sendData(QJsonDocument(json_));

        onFSM();
        callEvent();
        global_data_.push_back(processParams.filtered);

        temperature_data_.push_back(temperature_);
        work_tick_++;
    }
    else {
        offFSM();
        work_tick_ = 0;
        if(isLoggerCreated) {
            Logger::getInstance().log(global_data_, temperature_data_);
            global_data_.clear();
            isLoggerCreated = false;
        }
    }
    

    QCoreApplication::processEvents();
    
    
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
    print(coeffs);
    qDebug() << "Size: " <<  active_event_->end_time_mark_ - active_event_->start_time_mark_;
    auto fitData = applyFunc(
            std::vector<double>(std::begin(coeffs), std::end(coeffs)),
            0,
            active_event_->end_time_mark_ - active_event_->start_time_mark_,
            active_event_->end_time_mark_ - active_event_->start_time_mark_,
            gaussPolyVal
    ).second;

    auto max_el_it = std::max_element(std::begin(fitData), std::end(fitData));
    std::cout << "MAX: " << std::distance(std::begin(fitData), max_el_it)+start_mark_ << std::endl;
    auto vapor_point = std::find_if(max_el_it, std::end(fitData), [](auto val){return almostEqual(val, 0.95, 0.01);});
    std::cout << "VAPOR: " << std::distance(std::begin(fitData), vapor_point)+start_mark_ << std::endl;
    std::cout << "COND TEMP: " << temperature_data_[start_mark_] << std::endl;
    std::cout << "VAPOR TEMP: " << temperature_data_[std::distance(std::begin(fitData), vapor_point)+start_mark_] << std::endl;
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
        setRate = 1.7;
    break;

    case EventType::CALIBRATION:
        active_event_ = std::make_unique<Calibration>(process_unit_);
        setRate = -1.7;
    break;

    case EventType::MEASHUREMENT:
        if(process_unit_->getCalcParams().event_completeness.calibration) 
        {
            active_event_ = std::make_unique<Meashurement>(process_unit_);
        }
        
        setRate = -1.7;
    break;

    case EventType::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_, temperature_);
        setRate = -1.7;
        start_mark_ = process_unit_->getTick();
    break;

    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
        setRate = 1.7;
        Q_EMIT runOptimizationProcess(std::vector<double>(std::begin(global_data_) + active_event_->start_time_mark_, std::end(global_data_)));
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

