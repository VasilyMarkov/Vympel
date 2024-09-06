#include <iostream>
#include "bluetoothDevice.hpp"
#include <qt6/QtCore/QDebug>
#include <qt6/QtBluetooth/qbluetoothaddress.h>
#include <qt6/QtBluetooth/qbluetoothlocaldevice.h>
#include <qt6/QtBluetooth/qbluetoothuuid.h>

using namespace app;

// const QString g_device("Keychron K8");
// const QString g_device("M2 IE Free");
const QString g_device("RigCom");


DeviceDiscovery::DeviceDiscovery() {
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this, &DeviceDiscovery::addDevice);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
        this, &DeviceDiscovery::scanFinished);
}


void DeviceDiscovery::addDevice(const QBluetoothDeviceInfo& device) {
    qDebug() << device.address().toString() + u' ' + device.name();
    devices_.emplace(device.name(), device.address());

    if(device.name() == g_device) scanFinished();
}

void DeviceDiscovery::startScan() {
    discoveryAgent->start();
}

void DeviceDiscovery::scanFinished() {
    qDebug() << "Scan finished";
    discoveryAgent->stop();
    serviceDiscoveryAgent = new ServiceDiscovery(devices_.at(g_device), this);
}

QBluetoothAddress DeviceDiscovery::getDeviceAddress() {return devices_.at(g_device);}

ServiceDiscovery::ServiceDiscovery(const QBluetoothAddress& address, QObject *parent = nullptr) {
    QBluetoothLocalDevice localDevice;
    QBluetoothAddress adapterAddress = localDevice.address();
    discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress, this);
    discoveryAgent->setRemoteAddress(address);

    connect(discoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered,
            this, &ServiceDiscovery::addService);

    discoveryAgent->start();
}

void ServiceDiscovery::addService(const QBluetoothServiceInfo& service) {
    if (service.serviceName().isEmpty())
        return;
    QString line = service.serviceName();
    if (!service.serviceDescription().isEmpty())
        line.append("\n\t" + service.serviceDescription());
    if (!service.serviceProvider().isEmpty())
        line.append("\n\t" + service.serviceProvider());
    qDebug() << line;
}


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
    qDebug() << device_addr.toString();
    QBluetoothServiceDiscoveryAgent *serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(device_addr);
    connect(serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered,
        this, &BluetoothDevice::serviceDiscovered);

    connect(serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::finished,
        [this](){std::cout << "service finished" << std::endl;});
    connect(serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::errorOccurred,
        [this](){std::cout << "service error" << std::endl;});
    serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    // serviceDiscoveryAgent->errorString();
    // auto selectedDevice = devices_.at(g_device);
    // socket_ = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    // socket_->connectToService(QBluetoothAddress(selectedDevice),
    //                         QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::SerialPort));
    // connect(socket_, &QBluetoothSocket::errorOccurred, this, &BluetoothDevice::SocketError);
    // connect(socket_, &QBluetoothSocket::connected, this, &BluetoothDevice::SocketConnected);
    // connect(socket_, &QBluetoothSocket::disconnected, this, &BluetoothDevice::SocketDisconnected);
    // connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothDevice::SocketRead);

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
    connectToDevice(devices_.at(g_device));
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
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    devices_.emplace(device.name(), device.address());
}