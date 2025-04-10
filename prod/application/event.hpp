#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <deque>
#include <vector>
#include <iostream>
#include "interface.hpp"
#include "utility.hpp"
#include <functional>

namespace app
{

namespace constants {
    namespace buffer {
        constexpr size_t CALIB_SIZE = 100; 
        constexpr size_t MEASUR_SIZE = 100; 
    }
    constexpr size_t WAITING_TICKS = 100; 
    constexpr size_t SAMPLE_FREQ = 10; 
    constexpr size_t FRAME_DELAY_MS = 10;
}

class Event {
public:
    Event(std::weak_ptr<IProcessing>);
    virtual std::optional<EventType> operator()() = 0;
    virtual ~Event(){}
    size_t v_tick = 0;
protected:
    std::weak_ptr<IProcessing> process_unit_;
    std::vector<double> data_;
    inline static std::vector<double> global_data_;
    size_t start_tick_ = 0;

    std::deque<double> mean_data_;
    std::deque<double> mean_deque_;
    size_t local_tick_ = 0;
};

class Idle final: public Event {
public:
    Idle(std::weak_ptr<IProcessing>, const double&);
    std::optional<EventType> operator()() override;
private:
    const double& temperature_;
};

class Calibration final: public Event {
public:
    Calibration(std::weak_ptr<IProcessing>);
    std::optional<EventType> operator()() override;
};

class Meashurement final: public Event {
private:
    bool positiveTrendDetection(const std::vector<double>&);
public:
    Meashurement(std::weak_ptr<IProcessing>);
    std::optional<EventType> operator()() override;
};

class Сondensation final: public Event {

    const double& temperature_;
    
public:
    Сondensation(std::weak_ptr<IProcessing>, const double&);
    std::optional<EventType> operator()() override;
};

class End final: public Event {
    std::vector<double> coeffs_;
    bool is_coeffs_ready_ = false;
public:
    End(std::weak_ptr<IProcessing>);
    std::optional<EventType> operator()() override;
    void setCoeffs(const std::vector<double>&);
};

inline double polyval(const std::vector<double>& coeffs, double x) noexcept {
    double tmp = 0.0;
    size_t degree = coeffs.size() - 1;
    for( auto&& coeff : coeffs) {
         tmp += coeff*std::pow(x, degree);
         --degree;
    }
    return tmp;
}

inline double gaussPolyVal(const std::vector<double>& coeffs, double x) noexcept {
    double a = coeffs[0];
    double mean = coeffs[1];
    double var = coeffs[2];
    double z = polyval(std::vector<double>(std::next(std::begin(coeffs), 3), std::end(coeffs)),(x-mean));
    return a*std::exp(-(0.5*z*z)/(var*var));
}

template<typename Func>
std::pair<std::vector<double>,std::vector<double>>  applyFunc(
    const std::vector<double>& coeffs,
    int begin,
    int end,
    size_t pointsNum,
    Func&& function)
{
    assert(end > begin);
    std::vector<double> x_data(pointsNum);
    double step = std::fabs(end-begin)/pointsNum;
    x_data[0] = begin;
    std::generate(std::next(std::begin(x_data)), std::end(x_data), [step, begin](){
        static double val = begin;
        return val += step;
    });

    std::vector<double> y_data;
    y_data.reserve(x_data.size());

    for(auto&& x : x_data) {
        y_data.push_back(function(coeffs, x));
    }
    return {x_data, y_data};
}

} //namespace app

inline bool almostEqual(double lhs, double rhs, double eps) {
    return std::fabs(lhs-rhs) < eps;
}

#endif //EVENT_H