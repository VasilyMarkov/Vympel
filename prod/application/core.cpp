#include <QThread>
#include <QCoreApplication>
#include <QtConcurrent>
#include "core.hpp"
#include "utility.hpp"
#include "logger.hpp"
#include "cv.hpp"
#include "math.hpp"

namespace app
{
 
Core::Core(std::shared_ptr<IProcessing> processModule): 
    process_unit_(processModule) 
{
    connect(&timer_, &QTimer::timeout, this, &Core::process);
    connect(&timer_, &QTimer::timeout, [this](){
        static size_t cnt = 0;
        if(cnt % 10 == 0) {
            Q_EMIT setRateTemprature(setRate);
            cnt = 0;
        }
        ++cnt;
    });
    double process_freq = ConfigReader::getInstance().get("parameters", "process_freq_Hz").toInt();
    timer_.start(1000./process_freq);
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

    

    

    if(statement_ == CoreStatement::work) {
        process_unit_->process();
        // Q_EMIT sendCompressedImage(std::static_pointer_cast<CameraProcessingModule>(process_unit_)->getBuffer());
        if(!isLoggerCreated) {
            
            isLoggerCreated = true;
        }

        auto processParams = process_unit_->getProcessParams();
        json_["brightness"] = processParams.brightness;
        json_["filtered"] = processParams.filtered;
        json_["mode"] = static_cast<int>(mode_);
        json_["statement"] = static_cast<int>(statement_);

        if(ConfigReader::getInstance().isBleEnable()) {
            json_["temperature"] = temperature_;
            temperature_data_.push_back(temperature_);
        }
        else {
            json_["temperature"] = processParams.temperature;
            temperature_data_.push_back(processParams.temperature);
        }

        Q_EMIT sendData(QJsonDocument(json_));
        global_data_.push_back(processParams.filtered);

        callEvent();
        work_tick_++;
    }
    else {

        if(isLoggerCreated) {

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
    if(statement_ == CoreStatement::work) {
        Logger::getInstance().createLog();
        onFSM();
    }
    else {
        Logger::getInstance().log(global_data_, temperature_data_);
        global_data_.clear();
        offFSM();
        work_tick_ = 0;
    }
}

void Core::receiveRateTemprature(double temperatureRate) noexcept {
    temperatureRate_ = temperatureRate;
}

void Core::receiveFitCoefficients(const std::vector<double>& coeffs)
{
    qDebug() << "Coeffs: ";
    print(coeffs);
    int begin = start_time_mark_;
    int end = end_time_mark_;

    auto fit_data = applyFunc(coeffs, 0, end-begin, end-begin, gaussPolyVal);

    auto maximum_points = maximum(fit_data);
    print(maximum_points);
    int second_maximum = *std::prev(std::end(maximum_points));

    double maximum_value = fit_data[second_maximum];

    auto vapor_point_it = std::find_if(std::begin(fit_data) + second_maximum, std::end(fit_data), [maximum_value](double val){return val < maximum_value * 0.95;});

    auto vapor_point = std::distance(std::begin(fit_data), vapor_point_it) + start_time_mark_;

    qDebug() << "Second maximum: " << second_maximum;
    qDebug() << "Vapor point: " << vapor_point;
    qDebug() << "Temp size: " << temperature_data_.size();
    qDebug() << "First temp: " << temperature_data_[start_time_mark_];
    qDebug() << "Second temp: " << temperature_data_[vapor_point];
    qDebug() << "Temperature: " << (temperature_data_[vapor_point] + temperature_data_[start_time_mark_]) / 2;
    // auto max_el_it = std::max_element(std::begin(fitData), std::end(fitData));
    // std::cout << "MAX: " << std::distance(std::begin(fitData), max_el_it)+start_mark_ << std::endl;
    // auto vapor_point = std::find_if(max_el_it, std::end(fitData), [](auto val){return almostEqual(val, 0.95, 0.01);});
    // std::cout << "VAPOR: " << std::distance(std::begin(fitData), vapor_point)+start_mark_ << std::endl;
    // std::cout << "COND TEMP: " << temperature_data_[start_mark_] << std::endl;
    // std::cout << "VAPOR TEMP: " << temperature_data_[std::distance(std::begin(fitData), vapor_point)+start_mark_] << std::endl;
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
            active_event_ = std::make_unique<Meashurement>(process_unit_, start_time_mark_);
        }
        setRate = -1.7;
    break;

    case EventType::CONDENSATION:
        active_event_ = std::make_unique<Сondensation>(process_unit_, temperature_, end_time_mark_);
        setRate = -1.7;
    break;

    case EventType::END:
        active_event_ = std::make_unique<End>(process_unit_);
        setRate = 1.7;
        Q_EMIT runOptimizationProcess(std::vector<double>(std::begin(global_data_) + start_time_mark_, std::end(global_data_)));
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

