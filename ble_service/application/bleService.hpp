#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <QUdpSocket>
#include <QCoreApplication>
#include <QThread>
#include "utility.hpp"
#include "bluetoothDevice.hpp"
#include "udp.hpp"


class BlService final: public QObject {
  Q_OBJECT
public:
  BlService();
private Q_SLOTS:
  void process();
public Q_SLOTS:
  void run();
private:
  QTimer* timer_;
  std::unique_ptr<app::ble::BLEInterface> bluetoothDevice_;
  std::unique_ptr<app::network::UdpSocket> socket_;
};

#endif //BLE_SERVICE_H