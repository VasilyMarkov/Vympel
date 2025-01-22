#ifndef APP_H
#define APP_H

#include <QUdpSocket>
#include <QCoreApplication>
#include <QThread>
#include "core.hpp"
#include "udp.hpp"
#include "cv.hpp"
#include "utility.hpp"
#include "bluetoothDevice.hpp"
#include "logger.hpp"
#include "configReader.hpp"

namespace app
{
class Application final: public QObject {
    Q_OBJECT
public:
    Application(const QCoreApplication&);
    ~Application();
private:
    std::unique_ptr<UdpSocket> socket_;
    std::unique_ptr<UdpSocket> ble_socket_;
    std::unique_ptr<Core> core_;
    std::unique_ptr<ble::BLEInterface> bluetoothDevice_;
    QThread core_thread_;
};

} // namespace app


#endif //APP_H