#include <iostream>
#include "bluetoothDevice.hpp"
#include <qt6/QtCore/QDebug>

using namespace app;

const QString g_device("Redmi Note 11 Pro 5G");
// const QString device("M2 IE Free");
// const QString device("RigCom");

BluetoothDevice::BluetoothDevice()
{

}

BluetoothDevice::~BluetoothDevice()
{
    
}

void app::BluetoothDevice::startDeviceDiscovery()
{
    auto localDevice = new QBluetoothLocalDevice(this);

    connect(&discoveryAgent_, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDevice::deviceDiscovered);
    connect(&discoveryAgent_, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BluetoothDevice::scanFinished);

    discoveryAgent_.start(); 
    // std::cout << devices_.at(device).toString().toStdString() << std::endl; 
    
}

void app::BluetoothDevice::connectToDevice(const QBluetoothAddress& device_addr)
{
    // QBluetoothServiceDiscoveryAgent *serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(device_addr);
    // connect(serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered,
    //     this, &BluetoothDevice::serviceDiscovered);
    
    // serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    // serviceDiscoveryAgent->errorString();
    auto selectedDevice = devices_.at(g_device);
    socket_ = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    socket_->connectToService(QBluetoothAddress(selectedDevice),
                            QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::SerialPort));
    connect(socket_, &QBluetoothSocket::errorOccurred, this, &BluetoothDevice::SocketError);
    connect(socket_, &QBluetoothSocket::connected, this, &BluetoothDevice::SocketConnected);
    connect(socket_, &QBluetoothSocket::disconnected, this, &BluetoothDevice::SocketDisconnected);
    connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothDevice::SocketRead);

    // socket_ = new QBluetoothSocket(QBluetoothServiceInfo::UnknownProtocol, this);
}

void app::BluetoothDevice::scanFinished()
{
    
    // std::cout << device.toStdString() << ": " << devices_.at(device).toString().toStdString() << std::endl;
    // connectToDevice(devices_.at(device));


    // auto listOfDevices = discoveryAgent_.discoveredDevices();
    // for(auto&& device : listOfDevices) {
    //     qDebug() << device.name().trimmed() << device.address().toString().trimmed();
    // }
    std::cout << "Bluetooth scan finished." << std::endl;
    connectToDevice(QBluetoothAddress("hello"));
}

void app::BluetoothDevice::serviceDiscovered(const QBluetoothServiceInfo &service)
{
    qDebug() << "Found new service:" << service.serviceName()
             << '(' << service.device().address().toString() << ')';
}

void app::BluetoothDevice::SocketError(QBluetoothSocket::SocketError error)
{
    qDebug() << error;
}

void app::BluetoothDevice::SocketConnected()
{
}

void app::BluetoothDevice::SocketDisconnected()
{
}

void app::BluetoothDevice::SocketRead()
{
    qDebug() << socket_->readAll();
}

void BluetoothDevice::deviceDiscovered(const QBluetoothDeviceInfo & device)
{
    if(device.name() == g_device) {
        discoveryAgent_.stop(); 
        return;
    } 

    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    devices_.emplace(device.name(), device.address());
}