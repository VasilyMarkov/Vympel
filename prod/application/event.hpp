#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <deque>
#include <vector>
#include <iostream>
#include <functional>
#include "interface.hpp"
#include "utility.hpp"

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

class Event: public QObject {
    Q_OBJECT
public:
    Event(std::weak_ptr<IProcessing>);
    virtual std::optional<EventType> operator()() = 0;
    virtual ~Event(){}
    int start_time_mark_{};
    int end_time_mark_{};
    bool is_start_mark_ready_ = false;
    bool is_end_mark_ready_ = false;
    size_t mean_deque_size_;
    void setTick(size_t);
Q_SIGNALS:
    void sendStartMark(int);
    void sendEndMark(int);

protected:
    std::weak_ptr<IProcessing> process_unit_;
    std::vector<double> data_;
    inline static std::vector<double> global_data_;
    size_t start_tick_{};

    std::deque<double> mean_data_;
    std::deque<double> mean_deque_;
    size_t local_tick_{};
    size_t global_tick_{};
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
    bool detectGrowing(double mean);
    size_t start_grow_time_mark_{};
public:
    Meashurement(std::weak_ptr<IProcessing>, int&);
    std::optional<EventType> operator()() override;
    int& start_time_mark_;
};

class Сondensation final: public Event {
    const double& temperature_;
    bool detectStable(double mean);
public:
    Сondensation(std::weak_ptr<IProcessing>, const double&, int&);
    std::optional<EventType> operator()() override;
    int& end_time_mark_;
};

class End final: public Event {
    std::vector<double> coeffs_;
    bool is_coeffs_ready_ = false;
public:
    End(std::weak_ptr<IProcessing>);
    std::optional<EventType> operator()() override;
    void setCoeffs(const std::vector<double>&);
};

} //namespace app

#endif //EVENT_H