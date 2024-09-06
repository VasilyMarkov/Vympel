#pragma once
#include <qt6/QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <qt6/QtBluetooth/QBluetoothDeviceInfo>
#include <qt6/QtBluetooth/QBluetoothServiceDiscoveryAgent>
#include <qt6/QtBluetooth/QBluetoothSocket>
#include <qt6/QtBluetooth/QBluetoothLocalDevice>
#include <qt6/QtCore/QObject>
#include <memory>
#include <unordered_map>
namespace app {


class ServiceDiscovery final: public QObject {
  Q_OBJECT
public:
  ServiceDiscovery(const QBluetoothAddress&, QObject*);
public slots:
  void addService(const QBluetoothServiceInfo&);
private:
  QBluetoothServiceDiscoveryAgent *discoveryAgent;  
};

class DeviceDiscovery final: public QObject {
  Q_OBJECT
public:
  DeviceDiscovery();
  QBluetoothAddress getDeviceAddress();
public slots:
  void addDevice(const QBluetoothDeviceInfo&);
public slots:
  void startScan();
  void scanFinished();
private:
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  ServiceDiscovery *serviceDiscoveryAgent;
  std::unordered_map<QString, QBluetoothAddress> devices_;
};




class BluetoothDevice final: public QObject {
private:
  Q_OBJECT
public:
  BluetoothDevice();
  ~BluetoothDevice();
  void startDeviceDiscovery();
  void connectToDevice(const QBluetoothAddress&);
private Q_SLOTS:
  void deviceDiscovered(const QBluetoothDeviceInfo&);
  void scanFinished();
  void serviceDiscovered(const QBluetoothServiceInfo&);
  void SocketError(QBluetoothSocket::SocketError);
  void SocketConnected();
  void SocketDisconnected();
  void SocketRead();
private:
  QBluetoothDeviceDiscoveryAgent discoveryAgent_;
  std::unordered_map<QString, QBluetoothAddress> devices_;
  QBluetoothSocket* socket_;
};

}