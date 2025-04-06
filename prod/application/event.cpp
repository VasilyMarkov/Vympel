#include "event.hpp"
#include "logger.hpp"
#include "math.hpp"
#include <Eigen/Dense>
namespace app
{

Event::Event(std::weak_ptr<IProcessing> cv):
    process_unit_(cv), 
    start_tick_(process_unit_.lock()->getTick()),
    mean_data_(50, 0), mean_deque_(5,0) {}

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
    Event(cv)
{
    std::cout << "measure" << std::endl;
    qDebug() << process_unit_.lock()->getCalcParams().mean_filtered << process_unit_.lock()->getCalcParams().std_dev_filtered;

    // coeffs.resize(5);
}

std::optional<EventType> Meashurement::operator()()
{
    auto filtered = process_unit_.lock()->getProcessParams().filtered;

    global_data_.push_back(filtered);

    auto mean = process_unit_.lock()->getCalcParams().mean_filtered;
    auto std = process_unit_.lock()->getCalcParams().std_dev_filtered;
    auto threshold = mean + std*20;
    mean_data_.pop_front();
    mean_data_.push_back(filtered);
    static int d_cnt = 0;
    static int grow_cnt = 0;

    double diff{};
    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
        mean_deque_.push_back(mean_window);
        if (d_cnt > 4) {
            diff = *(std::end(mean_deque_)-1) - *(std::begin(mean_deque_));

            mean_deque_.pop_front();
        }
        if (diff > threshold) {
            ++grow_cnt;
        } else {
            grow_cnt = 0;
        }
        if (grow_cnt > 3) {
            return EventType::CONDENSATION; 
        }
        qDebug() << grow_cnt;
        ++d_cnt;
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
    Event(cv),  temperature_(temperature)
{
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

    double diff{};
    if(local_tick_ == mean_data_.size() - 1) {
        auto mean_window = std::accumulate(std::begin(mean_data_), std::end(mean_data_), 0.0) / mean_data_.size();
        mean_deque_.push_back(mean_window);
        diff = *(std::end(mean_deque_)-1) - *(std::begin(mean_deque_));
        mean_deque_.pop_front();

        if (process_unit_.lock()->getTick() > 2200) {
            return EventType::END; 
        }

        // if (diff < threshold) {
        //     ++non_grow_cnt;
        // } else {
        //     non_grow_cnt = 0;
        // }
        // if (non_grow_cnt > 3) {
        //     return EventType::END; 
        // }
        // qDebug() << non_grow_cnt;
        // ++d_cnt;
        local_tick_ = 0;
    }

    ++local_tick_;

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