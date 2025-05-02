#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include <QByteArray>
#include <QJsonDocument>
#include "Iir.h"

namespace app {

enum class EventType {
    NO_STATE,
    IDLE,
    CALIBRATION,
    MEASHUREMENT,
    CONDENSATION,
    END
};

struct process_params_t {
    double brightness = 0;
    double filtered = 0;    
    double temperature = 0;    
};

struct calc_params_t {
    double mean_filtered = 0;
    double std_dev_filtered = 0;

    struct event_completeness_t {
        bool calibration = false;
        bool MEASHUREMENT = false;
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
    Iir::Butterworth::LowPass<4> btFilter_;

public:
    enum class state {
        WORKING,
        NODATA,
        DONE
    };
    virtual state process() = 0;
    size_t getTick() const noexcept {return global_tick_;}
    process_params_t getProcessParams() const noexcept {return process_params_;}
    calc_params_t& getCalcParams() noexcept {return calc_params_;}
    virtual ~IProcessing(){}
};

class IReceiver {
public:
    virtual void receiveData(const QJsonDocument&) = 0;
    virtual ~IReceiver(){}
};

class ISender {
public:
    virtual void sendData(const QJsonDocument&) const = 0;;
    virtual ~ISender(){}
};

}

#endif //IO_INTERFACE_H

