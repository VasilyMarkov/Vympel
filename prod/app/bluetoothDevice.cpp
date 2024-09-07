#include <iostream>
#include "bluetoothDevice.hpp"
#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include <QBluetoothSocket>
#include <QModbusRtuSerialServer>
#include <QModbusDataUnit>
#include <QModbusReply>


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

using namespace ble;

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
    m_control(0),
    m_service(0),
    m_readTimer(0),
    m_connected(false),
    m_currentService(0)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLEInterface::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLEInterface::onScanFinished);
}

BLEInterface::~BLEInterface()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    delete device_;
}

void BLEInterface::scanDevices()
{
    m_deviceDiscoveryAgent->start();
    Q_EMIT statusInfoChanged("Scanning for devices...", true);
}

void BLEInterface::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qDebug() << device.name() << " Address: " << device.address().toString();
        // DeviceInfo *dev = new DeviceInfo(device);
        // m_devices.append(dev);
        device_ = new DeviceInfo(device);
    }
}

void BLEInterface::connectCurrentDevice()
{
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = 0;

    }
    m_control = QLowEnergyController::createCentral(device_->getDevice(), this);

    connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this, SLOT(onServiceDiscovered(QBluetoothUuid)));
    connect(m_control, SIGNAL(discoveryFinished()),
            this, SLOT(onServiceScanDone()));
    connect(m_control, SIGNAL(errorOccurred(QLowEnergyController::Error)),
            this, SLOT(onControllerError(QLowEnergyController::Error)));
    connect(m_control, SIGNAL(connected()),
            this, SLOT(onDeviceConnected()));
    connect(m_control, SIGNAL(disconnected()),
            this, SLOT(onDeviceDisconnected()));

    m_control->connectToDevice();
}

void BLEInterface::onDeviceConnected()
{
    m_control->discoverServices();
}

void BLEInterface::onServiceDiscovered(const QBluetoothUuid &gatt)
{
    Q_UNUSED(gatt)
    emit statusInfoChanged("Service discovered. Waiting for service scan to be done...", true);
}

void BLEInterface::onServiceScanDone()
{
    m_servicesUuid = m_control->services();
    if(m_servicesUuid.isEmpty())
        emit statusInfoChanged("Can't find any services.", true);
    else{
        const QString RX_SERVICE_UUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
        // auto it = std::find_if(std::begin(m_servicesUuid), std::end(m_servicesUuid), 
        //     [&RX_SERVICE_UUID](QBluetoothUuid uuid){return uuid.toString() == RX_SERVICE_UUID;});
        // if(it != std::end(m_servicesUuid)) {
        //     qDebug() << (*it).toString();
        //     m_control->createServiceObject(QBluetoothUuid(RX_SERVICE_UUID));

        // }
        for(auto&& service: m_servicesUuid) {
            qDebug() << service.toString();
        }
    }
}

void BLEInterface::onDeviceDisconnected()
{
    update_connected(false);
    qDebug() << "Remote device disconnected";
}

void BLEInterface::onControllerError(QLowEnergyController::Error error)
{
    qWarning() << "Controller Error:" << error;
}


void BLEInterface::onScanFinished()
{
    qDebug() << "Scan is done";
    connectCurrentDevice();
}

void BLEInterface::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qWarning() << "The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qWarning() << "Writing or reading from the device resulted in an error.";
    else
        qWarning() << "An unknown error has occurred.";
}


bool BLEInterface::isConnected() const {
    return m_connected;
}

int BLEInterface::currentService() const {
    return m_currentService;
}

void BLEInterface::setCurrentService(int currentService) {
    if (m_currentService == currentService)
        return;
    update_currentService(currentService);
    m_currentService = currentService;
    emit currentServiceChanged(currentService);
}

void BLEInterface::update_connected(bool connected){
    if(connected != m_connected){
        m_connected = connected;
        emit connectedChanged(connected);
    }
}

void BLEInterface::update_currentService(int indx)
{
    // delete m_service;
    // m_service = 0;

    // if (indx >= 0 && m_servicesUuid.count() > indx) {
    //     m_service = m_control->createServiceObject(
    //                 m_servicesUuid.at(indx), this);
    // }

    // if (!m_service) {
    //     emit statusInfoChanged("Service not found.", false);
    //     return;
    // }

    // connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
    //         this, SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)));
    // connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
    //         this, SLOT(onCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
    // connect(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)),
    //         this, SLOT(onCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));
    // connect(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)),
    //         this, SLOT(onCharacteristicWrite(QLowEnergyCharacteristic,QByteArray)));
    // connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)),
    //         this, SLOT(serviceError(QLowEnergyService::ServiceError)));

    // if(m_service->state() == QLowEnergyService::DiscoveryRequired) {
    //     emit statusInfoChanged("Connecting to service...", true);
    //     m_service->discoverDetails();
    // }
    // else
    //     searchCharacteristic();
}