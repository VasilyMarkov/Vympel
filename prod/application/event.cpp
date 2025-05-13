#include "event.hpp"
#include "logger.hpp"
#include "math.hpp"

#include <Eigen/Dense>
namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):
    process_unit_(cv), 
    start_tick_(process_unit_.lock()->getTick()),
    mean_data_(ConfigReader::getInstance().get("parameters", "mean_window_size").toInt(), 0), 
    mean_deque_size_(ConfigReader::getInstance().get("parameters", "mean_deque_size").toInt()), 
    mean_deque_(mean_deque_size_, 0) {}

void Event::setTick(size_t tick) {
    global_tick_ = tick;
}

Idle::Idle(std::weak_ptr<IProcessing> cv, const double& temperature): 
    Event(cv),
    temperature_(temperature)
{  
    std::cout << "idle" << std::endl;
    global_data_.clear();
}

std::optional<EventType> Idle::operator()()
{
    if(ConfigReader::getInstance().isBleEnable() && temperature_ < 52.0) {
        return std::nullopt;
    }
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
    auto brightness = process_unit_.lock()->getProcessParams().brightness;
    global_data_.push_back(brightness);

    auto calib_size = ConfigReader::getInstance().get("parameters", "calibration_size_buffer").toInt();

    if(local_tick_ >= calib_size) 
    {
        process_unit_.lock()->getCalcParams().event_completeness.calibration = true;
        auto [mean, std_deviation] = meanVar(data_);
        process_unit_.lock()->getCalcParams().mean_filtered = mean;
        process_unit_.lock()->getCalcParams().std_dev_filtered = std_deviation;
        
        return EventType::MEASHUREMENT;
    }
    data_.push_back(brightness);
    ++local_tick_;
    return std::nullopt;
}

Meashurement::Meashurement(std::weak_ptr<IProcessing> cv, int& time_mark): 
    Event(cv), start_time_mark_(time_mark)
{
    std::cout << "measure" << std::endl;
    qDebug() << process_unit_.lock()->getCalcParams().mean_filtered << process_unit_.lock()->getCalcParams().std_dev_filtered;
}

bool Meashurement::detectGrowing(double mean) {
    static size_t cnt{};
    static double prev_mean{};
    static std::deque<bool> window(ConfigReader::getInstance().get("parameters", "detect_growing_deque_size").toInt());

    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto std_factor = ConfigReader::getInstance().get("parameters", "std_factor").toInt();

    if(cnt > 0) {
        std::cout << "Mean: " << mean << ", Prev mean: " << prev_mean << std::endl;
        if(mean > prev_mean + std*std_factor) {
            window.push_back(true);
        }
        else {
            window.push_back(false);
        }
        window.pop_front();

        if(std::all_of(std::begin(window), std::end(window), [](bool val){return val == true;})) {
            return true;
        }
        print(window);
    }

    prev_mean = mean;
    ++cnt;
    return false;
}

std::optional<EventType> Meashurement::operator()()
{
    auto filtered = process_unit_.lock()->getProcessParams().filtered;

    global_data_.push_back(filtered);

    auto mean = process_unit_.lock()->getCalcParams().mean_filtered;
    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto threshold = mean + std*5;
    mean_data_.pop_front();
    mean_data_.push_back(filtered);
    static int d_cnt = 0;
    static int grow_cnt = 0;

    double diff{};
    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();

        if(detectGrowing(mean_window)) {
            std::cout << "COND POINT: " << global_tick_ << std::endl;
            start_time_mark_ = global_tick_;
            return EventType::CONDENSATION; 
        }

        local_tick_ = 0;
    }

    ++local_tick_;

    return std::nullopt;
}

bool Сondensation::detectStable(double mean) {
    static size_t cnt{};
    static double prev_mean{};
    static std::deque<bool> window(ConfigReader::getInstance().get("parameters", "detect_stable_deque_size").toInt());
    auto calib_mean = process_unit_.lock()->getCalcParams().mean_filtered;
    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto threshold = calib_mean + std;

    if(cnt > 0) {
        if(std::fabs(mean-prev_mean) > std / 4) {
            window.push_back(false);
        }
        else {
            window.push_back(true);
        }
        window.pop_front();

        if(std::all_of(std::begin(window), std::end(window), [](bool val){return val == true;})) {
            return true;
        }
    }
    print(window);
    prev_mean = mean;
    ++cnt;
    return false;
}

app::Сondensation::Сondensation(std::weak_ptr<IProcessing> cv, const double& temperature, int& time_mark): 
    Event(cv),  temperature_(temperature), end_time_mark_(time_mark)
{
    is_start_mark_ready_ = false;
    mean_data_.resize(20);
    std::cout << "condensation" << std::endl;
}

std::optional<EventType> app::Сondensation::operator()()
{   

    auto filtered = process_unit_.lock()->getProcessParams().filtered;

    global_data_.push_back(filtered);

    auto mean = process_unit_.lock()->getCalcParams().mean_filtered;
    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto threshold = mean + std*20;
    mean_data_.pop_front();
    mean_data_.push_back(filtered);
    static int d_cnt = 0;
    static int non_grow_cnt = 0;

    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
        mean_deque_.push_back(mean_window);
        mean_deque_.pop_front();

        if (std::all_of(std::begin(mean_deque_), std::end(mean_deque_), [threshold](const auto& val){ return val < threshold;})) {
            end_time_mark_ = global_tick_;
            std::cout << "END POINT: " << end_time_mark_ << std::endl;
            return EventType::END; 
        }

        local_tick_ = 0;
    }

    ++local_tick_;

    return std::nullopt;
}

End::End(std::weak_ptr<IProcessing> cv):
    Event(cv)
{
    is_end_mark_ready_ = false;
    std::cout << "End" << std::endl;
}

std::optional<EventType> End::operator()()
{
    return std::nullopt;
}

void End::setCoeffs(const std::vector<double>& coeffs)
{
    coeffs_ = coeffs;
    is_coeffs_ready_ = true;
}

} // namespace app