#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H
#include <qt5/QtCore/QByteArray>

namespace app {

namespace constants {
    namespace port {
        constexpr quint16 SENDER_PORT = 65000;
        constexpr quint16 RECEIVER_PORT = 65001;
    }
}

enum class core_mode_t {
    IDLE,
    CALIBRATION,
    MEASUREMENT,
    CONDENSATION
};

struct process_params_t {
    double brightness = 0;
    double filtered = 0;    
};

struct calc_params_t {
    double mean_filtered = 0;
    double std_dev_filtered = 0;

    struct event_completeness_t {
        bool calibration = false;
        bool measurement = false;
    };
    event_completeness_t event_completeness;

    enum class status_t {
        not_valid_filter_value,
        valid_filter_value
    };
    status_t status = status_t::not_valid_filter_value;
};

/**
 * @brief IProcessing
 * 
 * Provides processing interface
 */
class IProcessing {
protected:
    process_params_t process_params_;          //parameters obtained by machine vision algorithm 
    calc_params_t calc_params_;                //parameters obtained by processing cv parameters inside events
    size_t global_tick_ = 0;
public:
    virtual bool process() = 0;
    size_t getTick() const noexcept {return global_tick_;}
    process_params_t getProcessParams() const noexcept {return process_params_;}
    calc_params_t& getCalcParams() noexcept {return calc_params_;}
    virtual ~IProcessing(){}
};

class ICommunication {
public:
    virtual void receiveData(const QString&) = 0;
    virtual void sendData(const process_params_t&) const = 0;;
    virtual ~ICommunication(){}
};

}

#endif //IO_INTERFACE_H

