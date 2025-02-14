#ifndef BLUETOOTH_DEVICE_H
#define BLUETOOTH_DEVICE_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QTimer>
#include <QObject>
#include <memory>

namespace app 
{

static QByteArray createModbusPacket(uint16_t, uint16_t);
constexpr uint16_t TEMPERATURE_REGISTER = 68;
constexpr uint16_t IR  = 75;
constexpr uint16_t HR = 13;

namespace ble 
{

constexpr int READ_INTERVAL_MS = 3000;

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

Q_SIGNALS:
    void deviceChanged();
    
private:
    QBluetoothDeviceInfo m_device;
};


class BLEInterface final : public QObject
{
    Q_OBJECT
public:
    explicit BLEInterface(QObject *parent = 0);
    void writeDataToCharachteristic(const QByteArray&);
    void temperature();
public Q_SLOTS:
    void requestTemperature();
    void run();
    void changeRateTemprature(double);
Q_SIGNALS:
    void sendTemperature(double);
    void deviceConnected();
    void isReady();
private Q_SLOTS:
/*******************QBluetothDeviceDiscoveryAgent********************/
    void addDevice(const QBluetoothDeviceInfo&);
    void onScanFinished();
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
/********************************************************************/

/***********************QLowEnergyController*************************/
    void onServiceDiscovered(const QBluetoothUuid &);
    void onServiceScanDone();
    void onControllerError(QLowEnergyController::Error);
    void onDeviceConnected();
    void onDeviceDisconnected();
/********************************************************************/

/*************************QLowEnergyService**************************/
    void onServiceStateChanged(QLowEnergyService::ServiceState);
    void onCharacteristicChanged(const QLowEnergyCharacteristic&, const QByteArray&);
    void onCharacteristicRead(const QLowEnergyCharacteristic&, const QByteArray&);
    void onCharacteristicWrite(const QLowEnergyCharacteristic&, const QByteArray&);
    void serviceError(QLowEnergyService::ServiceError);
    void read();
/********************************************************************/

    void searchCharacteristic();
private:
    void connectDevice();

private:
    QBluetoothDeviceDiscoveryAgent* deviceDiscoveryAgent_;
    std::unique_ptr<DeviceInfo> device_;
    QLowEnergyController* low_energy_controller_;
    QLowEnergyService* modbus_service_;
    QLowEnergyCharacteristic writeCharacteristic_;
    QLowEnergyCharacteristic readCharacteristic_;
    QLowEnergyService::WriteMode writeMode_;
    QLowEnergyDescriptor notificationDesc_;
    QTimer* readTimer_;
};

} //namespace ble

} //namespace app

#endif //BLUETOOTH_DEVICE_H