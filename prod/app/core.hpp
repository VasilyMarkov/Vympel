#pragma once
#include <iostream>
#include <memory>
#include <list>
#include <qt6/QtCore/QObject>
#include <unordered_map>
#include <functional>
#include <optional>
#include <deque>
#include "interface.hpp"

namespace app {

namespace constants {
    namespace buffer {
        constexpr size_t CALIB_SIZE = 500; 
        constexpr size_t MEASUR_SIZE = 100; 
    }
    constexpr size_t WAITING_TICKS = 100; 
    constexpr size_t SAMPLE_FREQ = 10; 
    constexpr size_t FRAME_DELAY = 10; //ms
}

class Event {
protected:
    std::weak_ptr<IProcessing> process_unit_;
    std::vector<double> data_;
    size_t start_tick_ = 0;
public:
    Event(std::weak_ptr<IProcessing>);
    virtual std::optional<core_mode_t> operator()() = 0;
    virtual ~Event(){}
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
    std::vector<double> m_data;
    std::deque<double> coeffs_;
    size_t local_tick_ = 0;
public:
    Measurement(std::weak_ptr<IProcessing>);
    std::optional<core_mode_t> operator()() override;
};


class Fsm final {
public:
    Fsm(std::weak_ptr<IProcessing>);
    void toggle(core_mode_t);
    void callEvent();
    void dispatchEvent();
private:
    core_mode_t mode_ = core_mode_t::IDLE;
    std::weak_ptr<IProcessing> process_unit_;
    std::unique_ptr<Event> active_event_ = nullptr;
};


class Core final: public QObject, public ICommunication {
    Q_OBJECT
    friend class CoreTest;
public:
    explicit Core(std::shared_ptr<IProcessing>);
    std::shared_ptr<IProcessing> getProcessUnit() const;
public slots:
    void receiveData(const QString&) override;
    bool process();
signals:
    void sendData(const process_params_t&) const override;
    void exit();
    
private:
    std::shared_ptr<IProcessing> process_unit_ = nullptr;
    std::unique_ptr<Fsm> fsm_ = nullptr;
    
    const std::unordered_map<QString, core_mode_t> events_;
};

} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::core_mode_t)
