#pragma once
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
#include <unordered_map>


namespace app 
{

namespace ble 
{

#define READ_INTERVAL_MS 3000

inline uint16_t Crc16(const std::vector<uint8_t>& buf, int len) {
    unsigned short nCRC16 = 0xFFFF;
    unsigned short tmp;

    if (!buf.empty() && len > 0 && len <= buf.size()) {
        for (int i = 0; i < len; i++) {
            tmp = static_cast<unsigned short>(0x00FF & buf[i]);
            nCRC16 ^= tmp;
            for (int k = 0; k < 8; k++) {
                tmp = static_cast<unsigned short>(nCRC16 & 0x0001);
                if (tmp == 0x0001) {
                    nCRC16 >>= 1;
                    nCRC16 ^= 0xA001;
                } else {
                    nCRC16 >>= 1;
                }
            }
        }
        return nCRC16;
    }
    return 0x0000;
}

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
    void write(const QByteArray&);
signals:

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