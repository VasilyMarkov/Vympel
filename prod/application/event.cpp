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

    // if(mean_data.size() == mean_data.capacity()) {
    //     auto mean = std::accumulate(std::begin(mean_data), std::end(mean_data), 0.0)/mean_data.size();

    //     auto isPositiveGrowth = [](double coeff)
    //     {
    //         static double local_coeff = 0;
    //         auto greater = coeff > 1.1*local_coeff;
    //         local_coeff = coeff;
    //         return greater;
    //     };

    //     coeffs.pop_front();
    //     coeffs.push_back(isPositiveGrowth(mean));

    //     auto cnt = std::count_if(std::begin(coeffs), std::end(coeffs), [](bool val){return val == true;});

    //     if(cnt > 3) {
    //         std::cout << "START: " << process_unit_.lock()->getTick() << std::endl;
    //         return EventType::CONDENSATION;   
    //     }

    //     mean_data.clear();
    // }

    // mean_data.push_back(filtered);

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
    auto filtered = process_unit_.lock()->getProcessParams().filtered;
    global_data_.push_back(filtered);

    if(mean_data.size() == mean_data.capacity()) {
        auto mean = std::accumulate(std::begin(mean_data), std::end(mean_data), 0.0)/mean_data.size();

        auto isNegativeGrowth = [](double coeff)
        {
            static double local_coeff = 0;
            auto greater = coeff < local_coeff;
            local_coeff = coeff;
            return greater;
        };

        coeffs.pop_front();
        coeffs.push_back(isNegativeGrowth(mean));

        auto cnt = std::count_if(std::begin(coeffs), std::end(coeffs), [](bool val){return val == true;});

        if(cnt > 3) {
            std::cout << "STOP: " << process_unit_.lock()->getTick() << std::endl;
            return EventType::END;   
        }

        mean_data.clear();
    }

    mean_data.push_back(filtered);
    
    ++local_tick_;

    return std::nullopt;
}

End::End(std::weak_ptr<IProcessing> cv):Event(cv)
{
    std::cout << "vaporization" << std::endl;

    auto max_it = std::max_element(std::begin(global_data_), std::end(global_data_));
    auto start_point = std::distance(std::begin(global_data_), max_it);
    std::cout << start_point << std::endl;
    std::vector analize_data(max_it, std::end(global_data_));
    std::vector<double> lower_bound;
    std::copy_if(std::begin(analize_data), std::end(analize_data), std::back_inserter(lower_bound), 
        [&max_it](double value){return value < 0.96*(*max_it) && value >  0.94*(*max_it);});
    v_tick = start_point +  lower_bound.size()/2;
    global_data_.clear();
}

std::optional<EventType> End::operator()()
{
    return std::nullopt;
}

} // namespace app