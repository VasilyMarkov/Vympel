#ifndef MATH_H
#define MATH_H

#include <Eigen/Dense>
#include <vector>
#include <algorithm> 
#include "details.hpp"

namespace app {

inline double linearRegression(const std::vector<double>& data) {
    using namespace Eigen;

    const size_t windowSize = data.size();
    std::vector<double> window;
    window.reserve(windowSize);

    const double max = *std::max_element(std::begin(data), std::end(data));

    std::transform(std::begin(data), std::end(data), std::back_inserter(window), [max](auto x){return x /= max;});
    // print(data);
    // print(window);

    auto x = VectorXd::LinSpaced(windowSize, 0, windowSize-1);
    auto y = Eigen::VectorXd::Map(data.data(), windowSize);

    MatrixXd A(windowSize, 2);
    A.col(0) = x;
    A.col(1) = VectorXd::Ones(windowSize);

    /* Solve system of linear equations ((A^t)*A)*x = (A^t)*y, where A = QR, using Householder reflections */
    Vector2d solution = A.householderQr().solve(y); 
    return solution[0];
}

}

#endif //MATH_H