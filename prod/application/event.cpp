#include "event.hpp"
#include "logger.hpp"
#include "math.hpp"
#include <Eigen/Dense>
namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):
    process_unit_(cv), 
    start_tick_(process_unit_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv, const double& temperature): 
    Event(cv),
    temperature_(temperature)
{  
    std::cout << "idle" << std::endl;
    global_data_.clear();
}

std::optional<EventType> Idle::operator()()
{
    // if(temperature_ < 52.0) return std::nullopt;

    return EventType::CALIBRATION;
}

Calibration::Calibration(std::weak_ptr<IProcessing> cv): 
    Event(cv)
{
    std::cout << "calib" << std::endl;
    data_.reserve(constants::buffer::CALIB_SIZE);
}

std::optional<EventType> Calibration::operator()()
{
    auto filtered = process_unit_.lock()->getProcessParams().filtered;
    global_data_.push_back(filtered);

    auto calib_size = ConfigReader::getInstance().get("parameters", "calibration_size_buffer").toInt();

    if(process_unit_.lock()->getTick() - start_tick_ >= calib_size) 
    {
        process_unit_.lock()->getCalcParams().event_completeness.calibration = true;
        auto [mean, std_deviation] = meanVar(data_);
        process_unit_.lock()->getCalcParams().mean_filtered = mean;
        process_unit_.lock()->getCalcParams().std_dev_filtered = std_deviation;
        
        return EventType::MEASHUREMENT;    
    }
    data_.push_back(filtered);

    return std::nullopt;
}

Meashurement::Meashurement(std::weak_ptr<IProcessing> cv): 
    Event(cv), mean_data_(30, 0)
{
    std::cout << "measure" << std::endl;
    coeffs.resize(5);
}

std::optional<EventType> Meashurement::operator()()
{
    auto filtered = process_unit_.lock()->getProcessParams().filtered;

    global_data_.push_back(filtered);

    auto mean = process_unit_.lock()->getCalcParams().mean_filtered;
    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto threshold = mean + std*4;
    mean_data_.pop_front();
    mean_data_.push_back(filtered);

    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
        if(mean_window > threshold) {
            return EventType::CONDENSATION;   
        }
        // if(positiveTrendDetection(std::vector<double>(std::begin(mean_data_), std::end(mean_data_)))) {
            // return EventType::CONDENSATION;   
        // }

        local_tick_ = 0;
    }

    ++local_tick_;
    return std::nullopt;
}

bool Meashurement::positiveTrendDetection(const std::vector<double>& data) {
    static uint8_t cnt = 0;
    double slope = linearRegression(data);
    std::cout << "Slope: " << slope << std::endl;
    if(slope > 0.1) {
        ++cnt;
    }
    else {
        cnt = 0;    
    }
    if(cnt == 3) return true;

    return false;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> cv, const double& temperature): 
    Event(cv), mean_data_(15, 0),
    temperature_(temperature)
{
    std::cout << "condensation" << std::endl;
    coeffs.resize(5);
}

std::optional<EventType> app::Сondensation::operator()()
{   
    qDebug() << process_unit_.lock()->getTick();
    if (process_unit_.lock()->getTick() > 2500) {
        return EventType::END;
    }

    // auto filtered = process_unit_.lock()->getProcessParams().filtered;

    // auto mean = process_unit_.lock()->getCalcParams().mean_filtered;
    // auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    // auto threshold = mean + std*4;

    // mean_data_.pop_front();
    // mean_data_.push_back(filtered);

    // if(local_tick_ == mean_data_.size() - 1) {
    //     auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
    //     if(mean_window < threshold) {
    //         std::cout << "END POINT: " << process_unit_.lock()->getTick() << std::endl;
    //         return EventType::END;   
    //     }
    //     local_tick_ = 0;
    // }
    // if(temperature_ > 50.0) return EventType::END;

    return std::nullopt;
}

End::End(std::weak_ptr<IProcessing> cv):
    Event(cv)
{
    std::cout << "End" << std::endl;
}

std::optional<EventType> End::operator()()
{
    return EventType::NO_STATE;    
    // return EventType::IDLE;    
}

} // namespace app