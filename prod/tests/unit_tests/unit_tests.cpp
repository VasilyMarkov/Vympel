#include <gtest/gtest.h>
#include <iostream>
#include "utility.hpp"
#include "processUnit.hpp"

using namespace app;
using namespace constants;

namespace test {

TEST(CalibTest, process) {

    // std::vector<double> test_data(constants::buffer::CALIB_SIZE, 3e6);
    // auto processUnit = std::make_shared<ProcessUnit>(test_data);
    // Calibration calib{processUnit};

    // for (size_t i = 0; i < test_data.size(); ++i) {
        // EXPECT_FALSE(calib());
        // processUnit->process();
    // }
    // EXPECT_TRUE(calib());
    // EXPECT_EQ(processUnit->getCalcParams().event_completeness.calibration, true);
    // EXPECT_NEAR(processUnit->getCalcParams().mean_filtered, 3e6, 1e-6);
}

} //namespace test

TEST(LeastSquares, coeff1) {
    std::vector<double> data = {0,1,2,3,4,5,6};
    auto coeff = findLineCoeff(data);

    EXPECT_NEAR(coeff, 1.0, 1e-6);
}
TEST(LeastSquares, coeff2) {
    std::vector<double> data = {0,-1,-2,-3,-4,-5,-6};
    auto coeff = findLineCoeff(data);

    EXPECT_NEAR(coeff, -1.0, 1e-6);
}

TEST(LeastSquares, coeff3) {
    std::vector<double> data = {0,2,4,6,8,10,12};
    auto coeff = findLineCoeff(data);

    EXPECT_NEAR(coeff, 2.0, 1e-6);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

