#include <iostream>
#include "bluetoothDevice.hpp"
#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include <QBluetoothSocket>
#include <QSerialPort>
#include <bitset>

using namespace app;

const QString g_device("RigCom");

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
    m_readTimer(0),
    m_connected(false),
    m_currentService(0)
{
    // m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(QBluetoothAddress(QString("54:F8:2A:32:6D:92")), this);
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLEInterface::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, [this](QBluetoothDeviceDiscoveryAgent::Error error){
        qDebug() << "Discovery agent error occured: " << error;});
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLEInterface::onScanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BLEInterface::onScanFinished);
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
        device_ = new DeviceInfo(device);
        if (device.name() == g_device) {
            m_deviceDiscoveryAgent->stop();
        }
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

void BLEInterface::readReady()
{

}


void BLEInterface::onServiceScanDone()
{
    m_servicesUuid = m_control->services();
    if(m_servicesUuid.isEmpty())
        emit statusInfoChanged("Can't find any services.", true);
    else{
        const QString RX_SERVICE_UUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");

        for(auto&& service: m_servicesUuid) {
            qDebug() << service.toString();
        }

        auto service = m_control->createServiceObject(QBluetoothUuid(RX_SERVICE_UUID));

        connect(service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(onCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
        connect(service, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)),
                this, SLOT(onCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));
        connect(service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)),
                this, SLOT(onCharacteristicWrite(QLowEnergyCharacteristic,QByteArray)));
        connect(service, SIGNAL(errorOccurred(QLowEnergyService::ServiceError)),
                this, SLOT(serviceError(QLowEnergyService::ServiceError)));

        if(service->state() == QLowEnergyService::DiscoveryRequired) {
	        service->discoverDetails();
        }
        connect(service, &QLowEnergyService::stateChanged, [service, this](QLowEnergyService::ServiceState state) {
                if (state == QLowEnergyService::ServiceDiscovered) {
                    qDebug() << "Service discovered.";

                    // // Find the custom Modbus RTU characteristic
                    const QString RX_CHAR_UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
                    auto characteristic = service->characteristic(QBluetoothUuid(RX_CHAR_UUID));
                    if (characteristic.isValid()) {
                        qDebug() << "Characteristic found.";
                        
                    }
                }
            });
    }
}

void BLEInterface::onCharacteristicRead(const QLowEnergyCharacteristic &c,
                                        const QByteArray &value){
    qDebug() << "Characteristic Read: " << value;
}

void BLEInterface::onCharacteristicChanged(const QLowEnergyCharacteristic &c,
                                           const QByteArray &value)
{
    qDebug() << "Characteristic Changed: " << value;
}

void BLEInterface::onCharacteristicWrite(const QLowEnergyCharacteristic &c,
                                          const QByteArray &value)
{
    qDebug() << "Characteristic Written: " << value;
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

void BLEInterface::serviceError(QLowEnergyService::ServiceError e)
{
    qWarning() << "Service error:" << e;
}