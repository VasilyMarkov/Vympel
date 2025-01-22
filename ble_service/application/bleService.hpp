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
signals:
  void temperature();
private Q_SLOTS:
  void process();
public Q_SLOTS:
  void run();
private:
  QTimer* timer_;
};

#endif //BLE_SERVICE_H