#pragma once
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <memory>
#include <unordered_map>

#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QTimer>

namespace app 
{

namespace ble 
{
class DeviceInfo final: public QObject
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

class BLEInterface final : public QObject
{
    Q_OBJECT
public:
    explicit BLEInterface(QObject *parent = 0);
signals:

private Q_SLOTS:
    //QBluetothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo&);
    void onScanFinished();
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    /*****************************/

    //QLowEnergyController
    void onServiceDiscovered(const QBluetoothUuid &);
    void onServiceScanDone();
    void onControllerError(QLowEnergyController::Error);
    void onDeviceConnected();
    void onDeviceDisconnected();
    /********************/
private:
    void connectDevice();

private:
    const QString DEVICE_NAME = "RigCom";
    QBluetoothDeviceDiscoveryAgent* deviceDiscoveryAgent_;
    std::unique_ptr<DeviceInfo> device_;
    QLowEnergyController* low_energy_controller_;
    QLowEnergyService* modbus_service_;
};

} //namespace ble

} //namespace app