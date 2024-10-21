#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <deque>
#include <vector>
#include <iostream>
#include "interface.hpp"
#include "utility.hpp"

namespace app
{

namespace constants {
    namespace buffer {
        constexpr size_t CALIB_SIZE = 500; 
        constexpr size_t MEASUR_SIZE = 100; 
    }
    constexpr size_t WAITING_TICKS = 100; 
    constexpr size_t SAMPLE_FREQ = 10; 
    constexpr size_t FRAME_DELAY_MS = 10;
}

class Event {
public:
    Event(std::weak_ptr<IProcessing>);
    virtual std::optional<core_mode_t> operator()() = 0;
    virtual ~Event(){}
protected:
    std::weak_ptr<IProcessing> process_unit_;
    std::vector<double> data_;
    size_t start_tick_ = 0;
};

class Idle final: public Event {
public:
    Idle(std::weak_ptr<IProcessing>);
    std::optional<core_mode_t> operator()() override;
};

class Calibration final: public Event {
public:
    Calibration(std::weak_ptr<IProcessing>);
    std::optional<core_mode_t> operator()() override;
};

class Measurement final: public Event {
    std::vector<double> mean_data;
    std::deque<bool> coeffs;
    size_t local_tick_ = 0;
public:
    Measurement(std::weak_ptr<IProcessing>);
    std::optional<core_mode_t> operator()() override;
};

class Сondensation final: public Event {
public:
    Сondensation(std::weak_ptr<IProcessing>);
    std::optional<core_mode_t> operator()() override;
};

} //namespace app

#endif //EVENT_H