#pragma once
#include <qt6/QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <qt6/QtCore/QObject>
#include <memory>

namespace app {

class BluetoothDevice: public QObject {
private:
  Q_OBJECT

public:
    BluetoothDevice();
    ~BluetoothDevice();

private slots:
  void deviceDiscovered(const QBluetoothDeviceInfo&);
private:
  std::unique_ptr<QBluetoothDeviceDiscoveryAgent> discoveryAgent;
};

}