#include "event.hpp"
#include "logger.hpp"
namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):
    process_unit_(cv), 
    start_tick_(process_unit_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv): 
    Event(cv)
{  
    std::cout << "idle" << std::endl;
    global_data_.clear();
}

std::optional<EventType> Idle::operator()()
{
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
        std::cout << mean << ' ' << std_deviation << std::endl;
        return EventType::MEASHUREMENT;    
    }
    data_.push_back(filtered);

    return std::nullopt;
}

Meashurement::Meashurement(std::weak_ptr<IProcessing> cv): 
    Event(cv), mean_data_(15, 0)
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
    // auto std = 20000;
    auto threshold = mean + std*ConfigReader::getInstance().get("parameters", "sigma_threshold").toInt();
        
    mean_data_.pop_front();
    mean_data_.push_back(filtered);

    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
        if(mean_window > threshold) {
            std::cout << "COND POINT: " << process_unit_.lock()->getTick() << std::endl;
            return EventType::CONDENSATION;   
        }
        local_tick_ = 0;
    }

    ++local_tick_;
    return std::nullopt;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> cv): 
    Event(cv)
{
    std::cout << "condensation" << std::endl;
    mean_data.reserve(100);
    coeffs.resize(5);
}

std::optional<EventType> app::Сondensation::operator()()
{
    if(process_unit_.lock()->getTick() >= 2000) {
        return EventType::END;
    }
    return std::nullopt;
}

End::End(std::weak_ptr<IProcessing> cv):Event(cv)
{
    std::cout << "End" << std::endl;
}

std::optional<EventType> End::operator()()
{
    return std::nullopt;
}

} // namespace app