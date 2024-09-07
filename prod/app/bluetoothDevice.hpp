#pragma once
#include <qt6/QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <qt6/QtBluetooth/QBluetoothDeviceInfo>
#include <qt6/QtBluetooth/QBluetoothServiceDiscoveryAgent>
#include <qt6/QtBluetooth/QBluetoothSocket>
#include <qt6/QtBluetooth/QBluetoothLocalDevice>
#include <QObject>
#include <memory>
#include <unordered_map>

#include <qt6/QtBluetooth/QLowEnergyController>
#include <qt6/QtBluetooth/QLowEnergyService>
#include <QTimer>

namespace app 
{

class ServiceDiscovery final: public QObject {
  Q_OBJECT
public:
  ServiceDiscovery(const QBluetoothAddress&, QObject*);
public Q_SLOTS:
  void addService(const QBluetoothServiceInfo&);
private:
  QBluetoothServiceDiscoveryAgent *discoveryAgent;  
};

class DeviceDiscovery final: public QObject {
  Q_OBJECT
public:
  DeviceDiscovery();
  QBluetoothAddress getDeviceAddress();
public Q_SLOTS:
  void addDevice(const QBluetoothDeviceInfo&);
public Q_SLOTS:
  void startScan();
  void scanFinished();
private:
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  ServiceDiscovery *serviceDiscoveryAgent;
  std::unordered_map<QString, QBluetoothAddress> devices_;
};

namespace ble 
{
class DeviceInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)
public:
    DeviceInfo(const QBluetoothDeviceInfo &device);
    void setDevice(const QBluetoothDeviceInfo &device);
    QString getName() const { return m_device.name(); }
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;

signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo m_device;
};

class BLEInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(int currentService READ currentService WRITE setCurrentService NOTIFY currentServiceChanged)

public:
    explicit BLEInterface(QObject *parent = 0);
    ~BLEInterface();

    void connectCurrentDevice();
    // void disconnectDevice();
    Q_INVOKABLE void scanDevices();
    // void write(const QByteArray& data);

    bool isConnected() const;

    int currentService() const;

public Q_SLOTS:
    void setCurrentService(int);
signals:
    void statusInfoChanged(QString info, bool isGood);
    void dataReceived(const QByteArray &data);
    void connectedChanged(bool connected);

    void currentServiceChanged(int currentService);

private Q_SLOTS:
    //QBluetothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo&);
    void onScanFinished();
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    //QLowEnergyController
    void onServiceDiscovered(const QBluetoothUuid &);
    void onServiceScanDone();
    void onControllerError(QLowEnergyController::Error);
    void onDeviceConnected();
    void onDeviceDisconnected();

    //QLowEnergyService
    // void onServiceStateChanged(QLowEnergyService::ServiceState s);
    // void onCharacteristicChanged(const QLowEnergyCharacteristic &c,
    //                           const QByteArray &value);
    // void serviceError(QLowEnergyService::ServiceError e);

    // void read();
    // void onCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value);
    // void onCharacteristicWrite(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void update_currentService(int currentSerice);

private:
    // inline void waitForWrite();
    void update_connected(bool);

    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QLowEnergyDescriptor m_notificationDesc;
    QLowEnergyController *m_control;
    QList<QBluetoothUuid> m_servicesUuid;
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_readCharacteristic;
    QLowEnergyCharacteristic m_writeCharacteristic;
    QList<DeviceInfo*> m_devices;
//    bool m_foundService;
    QTimer *m_readTimer;
    bool m_connected;
    // void searchCharacteristic();
    int m_currentService;
    QLowEnergyService::WriteMode m_writeMode;

    DeviceInfo* device_;
};
} //namespace ble


} //namespace app