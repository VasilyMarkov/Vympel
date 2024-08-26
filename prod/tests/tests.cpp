#include <gtest/gtest.h>
#include <iostream>
#include "core.hpp"
#include "utility.hpp"

using namespace app;
using namespace constants;

namespace test {

class TestProcessUnit final: public IProcessing {
    std::vector<double> test_data;
    LowPassFilter filter;
public:
    TestProcessUnit(const std::vector<double>& data):
        test_data(data), 
        filter(filter::cutoff_frequency, filter::sample_rate) {}
        
    bool process() override {
        process_params_.brightness = test_data[global_tick_];
        process_params_.filtered = filter.Process(process_params_.brightness);
        ++global_tick_;
    }
};

TEST(LowPassFilterTest, cutoff) {
    LowPassFilter filter(filter::cutoff_frequency, filter::sample_rate);

}

TEST(CalibTest, process) {

    std::vector<double> test_data(constants::buffer::CALIB_SIZE, 3e6);
    auto processUnit = std::make_shared<TestProcessUnit>(test_data);
    Calibration calib{processUnit};

    for (size_t i = 0; i < test_data.size(); ++i) {
        // EXPECT_FALSE(calib());
        // processUnit->process();
    }
    // EXPECT_TRUE(calib());
    // EXPECT_EQ(processUnit->getCalcParams().event_completeness.calibration, true);
    // EXPECT_NEAR(processUnit->getCalcParams().mean_filtered, 3e6, 1e-6);
}

} //namespace test

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

