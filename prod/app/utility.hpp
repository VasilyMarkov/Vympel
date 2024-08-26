#pragma once
#include <chrono>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <concepts>
#include <numeric>
#include <cmath>

namespace app {

template <typename T>
void print(const std::vector<T>& vector) {
    for(auto&& el:vector) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

template <typename T, typename U>
void print(const std::unordered_map<T, U>& map) {
    for(auto&& el:map) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

namespace constants {
    namespace filter {
        constexpr double cutoff_frequency = 10.0; //Hz
        constexpr double sample_rate = 1000.0; //Hz
    }
}

inline std::pair<double, double> mean_var(const std::vector<double>& window) {
    auto mean = std::accumulate(std::begin(window), std::end(window), 0.0)/window.size();
    auto sq_sum = std::inner_product(std::begin(window), std::end(window), std::begin(window), 0.0);
    return {mean, std::sqrt(sq_sum/window.size() - mean*mean)};
}

class LowPassFilter {
public:
    LowPassFilter(double cutoff_frequency, double sample_rate, double q = 0.707): 
        alpha_(std::sin(2 * M_PI * cutoff_frequency / sample_rate) / (2 * q)),
        y_(0) {}

    double Process(double x) {
        y_ = alpha_ * (x - y_) + y_;
        return y_;
    }

private:
    double alpha_;
    double y_;
};

}//namespace app