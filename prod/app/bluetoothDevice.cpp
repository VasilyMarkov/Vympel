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
    deviceDiscoveryAgent_(new QBluetoothDeviceDiscoveryAgent(this))
{
    connect(deviceDiscoveryAgent_, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BLEInterface::addDevice);
    connect(deviceDiscoveryAgent_, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &BLEInterface::onDeviceScanError);
    connect(deviceDiscoveryAgent_, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &BLEInterface::onScanFinished);

    deviceDiscoveryAgent_->start();
}

void BLEInterface::addDevice(const QBluetoothDeviceInfo& device) 
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        std::cout << device.name().toStdString() << std::endl;
        if(device.name() == DEVICE_NAME) {
            std::cout << "Find " << DEVICE_NAME.toStdString() << std::endl;
            device_ = std::make_unique<DeviceInfo>(device);

            deviceDiscoveryAgent_->stop();
        }
    }
}

void BLEInterface::onScanFinished()
{
    std::cout << "scan is finished" << std::endl;
    connectDevice();
}

void BLEInterface::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{

}

void BLEInterface::connectDevice() {

    if(!device_) {
        std::cout << "Device not found" << std::endl;
    }
    else {
        low_energy_controller_ = QLowEnergyController::createCentral(device_->getDevice(), this);

        connect(low_energy_controller_, &QLowEnergyController::serviceDiscovered,
            this, &BLEInterface::onServiceDiscovered);
        connect(low_energy_controller_, &QLowEnergyController::discoveryFinished,
            this, &BLEInterface::onServiceScanDone);
        connect(low_energy_controller_, &QLowEnergyController::errorOccurred,
            this, &BLEInterface::onControllerError);
        connect(low_energy_controller_, &QLowEnergyController::connected,
            this, &BLEInterface::onDeviceConnected);
        connect(low_energy_controller_, &QLowEnergyController::disconnected,
            this, &BLEInterface::onDeviceDisconnected);

        low_energy_controller_->connectToDevice();
    }
}

void BLEInterface::onServiceDiscovered(const QBluetoothUuid& uuid)
{
}

void BLEInterface::onServiceScanDone()
{
    auto uuid_services = low_energy_controller_->services();
    if(!uuid_services.isEmpty()) {
        for(auto&& service:uuid_services) {
            std::cout << service.toString().toStdString() << std::endl;
        }
    }
}

void BLEInterface::onControllerError(QLowEnergyController::Error error)
{
    std::cout << "Cannot connect to remote device." << std::endl;
    std::cout << "Controller Error: " << error << std::endl;
}

void BLEInterface::onDeviceConnected()
{
    low_energy_controller_->discoverServices();
}

void BLEInterface::onDeviceDisconnected()
{
}



}
} //namespace ble