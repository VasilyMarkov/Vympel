#ifndef APP_H
#define APP_H

#include <QUdpSocket>
#include <QCoreApplication>
#include <QThread>
#include <QProcess>
#include "core.hpp"
#include "network.hpp"
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
    void runCore();
    void runBle();
private:
    std::unique_ptr<UdpHandler> udp_handler_;
    std::unique_ptr<TcpHandler> tcp_handler_;
    std::unique_ptr<UdpHandler> ble_socket_;
    std::unique_ptr<Core> core_;
    std::unique_ptr<ble::BLEInterface> bluetoothDevice_;
    QThread core_thread_;
    QThread ble_thread_;
    QProcess camera_python_;
    const QCoreApplication& q_core_app_;
};

} // namespace app


#endif //APP_H