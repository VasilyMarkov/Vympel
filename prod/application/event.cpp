#include "event.hpp"

namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):process_unit_(cv), start_tick_(process_unit_.lock()->getTick()) {}

Idle::Idle(std::weak_ptr<IProcessing> cv): Event(cv)
{
}

std::optional<core_mode_t> Idle::operator()()
{
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
    if(process_unit_.lock()->getTick() - start_tick_ >= constants::buffer::CALIB_SIZE) 
    {
        // process_unit_.lock()->getCalcParams().mean_filtered = std::accumulate(std::begin(data_), std::end(data_), 0)/data_.size();
        process_unit_.lock()->getCalcParams().event_completeness.calibration = true;
        auto [mean, std_deviation] = meanVar(data_);
        process_unit_.lock()->getCalcParams().mean_filtered = mean;
        std::cout << mean << ' ' << std_deviation << std::endl;
        return core_mode_t::MEASUREMENT;    
    }
    data_.push_back(process_unit_.lock()->getProcessParams().filtered);

    return std::nullopt;
}

Measurement::Measurement(std::weak_ptr<IProcessing> cv): Event(cv)
{
    std::cout << "measure" << std::endl;
    data_.reserve(constants::buffer::MEASUR_SIZE);
    mean_data.reserve(100);
    coeffs.resize(5);
}

std::optional<core_mode_t> Measurement::operator()()
{
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
        std::cout << "tick: " << process_unit_.lock()->getTick() << ' ' << cnt << std::endl;
        std::cout << mean << std::endl;

        if(cnt > 3) {
            std::cout << "START" << ' ' << process_unit_.lock()->getTick() << std::endl;
            return core_mode_t::CONDENSATION;   
        }

        mean_data.clear();
    }

    mean_data.push_back(process_unit_.lock()->getProcessParams().filtered);


    ++local_tick_;
    return std::nullopt;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> process_unit): Event(process_unit)
{
    std::cout << "condensation" << std::endl;
}

std::optional<core_mode_t> app::Сondensation::operator()()
{
    return std::nullopt;
}

} // namespace app