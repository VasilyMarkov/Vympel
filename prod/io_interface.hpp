#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H
#include <qt6/QtCore/QByteArray>

namespace app {

namespace constants {
    constexpr quint16 SENDER_PORT = 65000;
    constexpr quint16 RECEIVER_PORT = 65001;
}

enum class core_mode_t {
    IDLE,
    CALIBRATION,
    MEASHUREMENT
};

enum class status_t {
    not_valid_filter_value,
    valid_filter_value
};

struct params_t {
    double brightness;
    double filtered;
    status_t status;
};


/// @brief Interface providing sending logic
class ISender {
public:
    virtual ~ISender(){};
    virtual void sendData(const QByteArray&) = 0;
};

/// @brief Interface providing receiving logic
class IReceiver {
public:
    virtual ~IReceiver(){};
    virtual void receiveData() = 0;
};

/// @brief Interface providing subscription to the Core class
class IObserver {
public:
  virtual ~IObserver(){};
  virtual void update(const params_t&) = 0;
};

class ISubject {
public:
    virtual ~ISubject(){};
    virtual void attach(std::unique_ptr<IObserver>) = 0;
    virtual void detach(std::unique_ptr<IObserver>) = 0;
    virtual void notify() const = 0;
};

}
#endif //IO_INTERFACE_H

