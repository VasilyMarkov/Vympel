#include <iostream>
#include "bluetoothDevice.hpp"
#include <qt6/QtCore/QDebug>


BluetoothDevice::BluetoothDevice(): discoveryAgent(std::make_unique<QBluetoothDeviceDiscoveryAgent>(this))
{
    connect(discoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDevice::deviceDiscovered);
    discoveryAgent->start();
}

BluetoothDevice::~BluetoothDevice()
{
    
}

void BluetoothDevice::deviceDiscovered(const QBluetoothDeviceInfo & device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
}
