#pragma once
#include <qt5/QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <qt5/QtCore/QObject>
#include <memory>

namespace app {

class BluetoothDevice: public QObject {
private:
  Q_OBJECT

public:
    BluetoothDevice();
    ~BluetoothDevice();

private Q_SLOTS:
  void deviceDiscovered(const QBluetoothDeviceInfo&);
private:
  std::unique_ptr<QBluetoothDeviceDiscoveryAgent> discoveryAgent;
};

}