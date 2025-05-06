#ifndef MATH_H
#define MATH_H

#include <Eigen/Dense>
#include <unsupported/Eigen/Polynomials>
#include <vector>
#include <algorithm> 
#include "details.hpp"
#include "utility.hpp"

namespace app {

inline double linearRegression(const std::vector<double>& data) {
    using namespace Eigen;

    const size_t windowSize = data.size();
    std::vector<double> window;
    window.reserve(windowSize);

    const double max = *std::max_element(std::begin(data), std::end(data));

    std::transform(std::begin(data), std::end(data), std::back_inserter(window), [max](auto x){return x /= max;});


    auto x = VectorXd::LinSpaced(windowSize, 0, windowSize-1);
    auto y = Eigen::VectorXd::Map(data.data(), windowSize);

    MatrixXd A(windowSize, 2);
    A.col(0) = x;
    A.col(1) = VectorXd::Ones(windowSize);

    /* Solve system of linear equations ((A^t)*A)*x = (A^t)*y, where A = QR, using Householder reflections */
    Vector2d solution = A.householderQr().solve(y); 
    return solution[0];
}


inline bool almostEqual(double lhs, double rhs, double eps) {
    return std::fabs(lhs-rhs) < eps;
}

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
std::vector<double> applyFunc(
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
    return y_data;
}

inline std::vector<double> polynomialDerivative(const std::vector<double>& coeffs) {
    std::vector<double> deriv_coeffs;
    for (size_t i = 0; i < coeffs.size(); ++i) {
        deriv_coeffs.push_back(coeffs[i] * (coeffs.size() - 1 - i));
    }
    return deriv_coeffs;
}

inline std::vector<double> derivative(const std::vector<double>& data) {
    std::vector<double> result(data);
    for (size_t i = 0; i < data.size()-1; ++i) {
        result[i] = data[i+1] - data[i];
    }
    return result;
}

inline std::vector<int> argmaximum(const std::vector<double>& data) {
    std::vector<int> maximum;
    for(size_t i = 0; i < data.size()-1; ++i) {
        if (data[i] >= 0 && data[i+1] < 0) {
            maximum.push_back(i);
        }
    }
    return maximum;
}

inline std::vector<int> maximum(const std::vector<double>& data) {
    return argmaximum(derivative(data));
}

}



#endif //MATH_H