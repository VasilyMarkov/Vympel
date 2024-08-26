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

inline std::vector<double> readInputData() {
    std::vector<double> data;
    double temp = 0;

    if(std::cin.fail()) throw std::runtime_error("Invalid input data");

    while (std::cin >> temp) {
        data.push_back(temp);
    }
    
    return data;
}

namespace constants {
    namespace filter {
        constexpr double cutoff_frequency = 10.0; //Hz
        constexpr double sample_rate = 1000.0; //Hz
    }
}


/**
 * @brief meanVar
 * 
 * Calculates mean and variance
 * 
 * @param  std::vector<double> 
 * @return std::pair<double, double> 
 */
inline std::pair<double, double> meanVar(const std::vector<double>& data) {
    auto mean = std::accumulate(std::begin(data), std::end(data), 0.0)/data.size();
    auto sq_sum = std::inner_product(std::begin(data), std::end(data), std::begin(data), 0.0);
    return {mean, std::sqrt(sq_sum/data.size() - mean*mean)};
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