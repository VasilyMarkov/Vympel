#include <iostream>
#include "bluetoothDevice.hpp"
#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include <QBluetoothSocket>
#include <QSerialPort>
#include <bitset>

namespace app 
{

namespace ble 
{

DeviceInfo::DeviceInfo(const QBluetoothDeviceInfo &info):
    QObject(), m_device(info) {}

QBluetoothDeviceInfo DeviceInfo::getDevice() const
{
    return m_device;
}

QString DeviceInfo::getAddress() const
{
    return m_device.address().toString();
}

void DeviceInfo::setDevice(const QBluetoothDeviceInfo &device)
{
    m_device = device;
    emit deviceChanged();
}


BLEInterface::BLEInterface(QObject *parent) : QObject(parent),
    deviceDiscoveryAgent_(std::make_unique<QBluetoothDeviceDiscoveryAgent>())
{
    connect(deviceDiscoveryAgent_.get(), &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BLEInterface::addDevice);

    connect(deviceDiscoveryAgent_.get(), &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &BLEInterface::onDeviceScanError);

    connect(deviceDiscoveryAgent_.get(), &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BLEInterface::onScanFinished);

    deviceDiscoveryAgent_->start();
    
}

BLEInterface::~BLEInterface()
{

}

void BLEInterface::addDevice(const QBluetoothDeviceInfo& device) 
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        std::cout << "Discovered LE Device name: " << device.name().toStdString() << std::endl;

        if(device.name() == DEVICE_NAME) {
            deviceDiscoveryAgent_->stop();
            std::cout << "Scan is done" << std::endl;
            device_ = std::make_unique<DeviceInfo>(device);
        }
    }
}

void app::ble::BLEInterface::onScanFinished()
{

}

void app::ble::BLEInterface::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{

}

}
} //namespace ble