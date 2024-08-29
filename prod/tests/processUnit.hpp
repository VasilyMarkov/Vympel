#pragma once

#include "interface.hpp"
#include "utility.hpp"

using namespace app;
using namespace constants;

namespace test {

/**
 * @brief Process Unit
 * This process unit uses for tests
 * Takes data from "test_data.dat" and imitates taken from camera data 
 */
class ProcessUnit final: public IProcessing {
    std::vector<double> test_data;
    LowPassFilter filter;
public:
    ProcessUnit():
        test_data(readInputData()), 
        filter(filter::cutoff_frequency1, filter::sample_rate) {}
        
    bool process() override {
        if(global_tick_ == test_data.size()) return false;

        process_params_.brightness = test_data[global_tick_];
        process_params_.filtered = filter.Process(process_params_.brightness);

        ++global_tick_;
        return true;
    }
};

} //namespace test