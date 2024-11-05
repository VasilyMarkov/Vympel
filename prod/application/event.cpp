#include "event.hpp"

namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):process_unit_(cv), start_tick_(process_unit_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv): Event(cv)
{  
}

std::optional<core_mode_t> Idle::operator()()
{
    global_data_.push_back(process_unit_.lock()->getProcessParams().filtered);
    if(process_unit_.lock()->getTick() - start_tick_ >= constants::WAITING_TICKS) {
        return core_mode_t::CALIBRATION;   
    }
    return std::nullopt;
}

Calibration::Calibration(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "calib" << std::endl;
    data_.reserve(constants::buffer::CALIB_SIZE);
}

std::optional<core_mode_t> Calibration::operator()()
{

    auto filtered = process_unit_.lock()->getProcessParams().filtered;
    global_data_.push_back(filtered);

    if(process_unit_.lock()->getTick() - start_tick_ >= constants::buffer::CALIB_SIZE) 
    {
        // process_unit_.lock()->getCalcParams().mean_filtered = std::accumulate(std::begin(data_), std::end(data_), 0)/data_.size();
        process_unit_.lock()->getCalcParams().event_completeness.calibration = true;
        auto [mean, std_deviation] = meanVar(data_);
        process_unit_.lock()->getCalcParams().mean_filtered = mean;
        std::cout << mean << ' ' << std_deviation << std::endl;
        return core_mode_t::MEASUREMENT;    
    }
    data_.push_back(filtered);

    return std::nullopt;
}

Measurement::Measurement(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "measure" << std::endl;
    mean_data.reserve(100);
    coeffs.resize(5);
}

std::optional<core_mode_t> Measurement::operator()()
{
    auto filtered = process_unit_.lock()->getProcessParams().filtered;
    global_data_.push_back(filtered);

    if(mean_data.size() == mean_data.capacity()) {
        auto mean = std::accumulate(std::begin(mean_data), std::end(mean_data), 0.0)/mean_data.size();

        auto isPositiveGrowth = [](double coeff)
        {
            static double local_coeff = 0;
            auto greater = coeff > local_coeff;
            local_coeff = coeff;
            return greater;
        };

        coeffs.pop_front();
        coeffs.push_back(isPositiveGrowth(mean));

        auto cnt = std::count_if(std::begin(coeffs), std::end(coeffs), [](bool val){return val == true;});

        if(cnt > 3) {
            std::cout << "START: " << process_unit_.lock()->getTick() << std::endl;
            return core_mode_t::CONDENSATION;   
        }

        mean_data.clear();
    }

    mean_data.push_back(filtered);


    ++local_tick_;
    return std::nullopt;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> process_unit): Event(process_unit)
{
    std::cout << "condensation" << std::endl;
    mean_data.reserve(100);
    coeffs.resize(5);
}

std::optional<core_mode_t> app::Сondensation::operator()()
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
            return core_mode_t::END;   
        }

        mean_data.clear();
    }

    mean_data.push_back(filtered);

    ++local_tick_;
    return std::nullopt;
}

End::End(std::weak_ptr<IProcessing> cv):Event(cv)
{
    auto max_it = std::max_element(std::begin(global_data_), std::end(global_data_));
    auto start_point = std::distance(std::begin(global_data_), max_it);
    std::cout << start_point << std::endl;
    std::vector analize_data(max_it, std::end(global_data_));
    std::vector<double> lower_bound;
    std::copy_if(std::begin(analize_data), std::end(analize_data), std::back_inserter(lower_bound), 
        [&max_it](double value){return value < 0.97*(*max_it) && value >  0.93*(*max_it);});

    std::cout << start_point +  lower_bound.size()/2 << std::endl;
}

std::optional<core_mode_t> End::operator()()
{
    return std::nullopt;
}

} // namespace app