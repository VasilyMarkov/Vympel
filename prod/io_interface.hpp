#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H
#include <qt6/QtCore/QByteArray>

namespace app {

namespace constants {
    constexpr quint16 SENDER_PORT = 65000;
    constexpr quint16 RECEIVER_PORT = 65001;
    constexpr size_t  RESERVE_SIZE = 10000;
    constexpr size_t  CALIBRATION_WINDOW = 200;
}

enum class core_mode_t {
    IDLE,
    CALIBRATION,
    MEASUREMENT
};

enum class status_t {
    not_valid_filter_value,
    valid_filter_value
};

struct params_t {
    double brightness = 0;
    double filtered = 0;
    status_t status = status_t::not_valid_filter_value;
};

}

#endif //IO_INTERFACE_H

