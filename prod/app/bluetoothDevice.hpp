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
    ~BLEInterface();

public Q_SLOTS:
    void addDevice(const QBluetoothDeviceInfo&);
    void onScanFinished();
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
signals:


private Q_SLOTS:

private:
    const QString DEVICE_NAME = "RigCom";
    std::unique_ptr<QBluetoothDeviceDiscoveryAgent> deviceDiscoveryAgent_;
    std::unique_ptr<DeviceInfo> device_;
};
} //namespace ble


} //namespace app