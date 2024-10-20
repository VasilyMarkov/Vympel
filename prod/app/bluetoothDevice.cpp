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

void BLEInterface::write(const QByteArray& data)
{
    
    const QString RX_CHAR_UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    auto characteristic = modbus_service_->characteristic(QBluetoothUuid(RX_CHAR_UUID));
    if (characteristic.isValid()) {
        modbus_service_->writeCharacteristic(characteristic, data);
    }
    if(modbus_service_ && writeCharacteristic_.isValid()) {
        modbus_service_->writeCharacteristic(writeCharacteristic_, data, writeMode_);
    }
}

void BLEInterface::addDevice(const QBluetoothDeviceInfo& device) 
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        QString DEVICE_NAME("RigCom");
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
            /*Сurly braces are required because low_energy_controller_ returns uuid exactly in this form*/
            QString RX_SERVICE_UUID = "{6e400001-b5a3-f393-e0a9-e50e24dcca9e}";
            if(service.toString() == RX_SERVICE_UUID) {
                modbus_service_ = low_energy_controller_->createServiceObject(service, this);
            }
        }

        if(modbus_service_) {
            connect(modbus_service_, &QLowEnergyService::stateChanged,
                this, &BLEInterface::onServiceStateChanged);
            connect(modbus_service_, &QLowEnergyService::characteristicChanged,
                this, &BLEInterface::onCharacteristicChanged);
            connect(modbus_service_, &QLowEnergyService::errorOccurred,
                this, &BLEInterface::serviceError);
            connect(modbus_service_, &QLowEnergyService::characteristicRead,
                this, &BLEInterface::onCharacteristicRead);
            connect(modbus_service_, &QLowEnergyService::characteristicWritten,
                this, &BLEInterface::onCharacteristicWrite);

            if(modbus_service_->state() == QLowEnergyService::DiscoveryRequired) {
                std::cout << "Discover details" << std::endl;
                modbus_service_->discoverDetails();
            }
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

void BLEInterface::onServiceStateChanged(QLowEnergyService::ServiceState service_state)
{
    if (service_state == QLowEnergyService::ServiceDiscovered) {

        QString RX_SERVICE_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
        auto characteristic = modbus_service_->characteristic(QBluetoothUuid(RX_SERVICE_UUID));
        if (characteristic.isValid()) {
            std::cout << "Characteristic found" << std::endl;

        }
        searchCharacteristic();

        QByteArray modbus;
        modbus.resize(6);
        modbus[0] = 0x01; //modbus address
        modbus[1] = 0x04; //function code
        modbus[2] = 0x00; //first register address MSB
        modbus[3] = 20; //first register address LSB
        modbus[4] = 0x00; //registers amount MSB
        modbus[5] = 0x02; //registers amount LSB

        std::vector<unsigned char> v_data(modbus.begin(), modbus.end());

        auto crc = Crc16(v_data, v_data.size());

        modbus.push_back(static_cast<char>(crc & 0xFF));       // Low byte
        modbus.push_back(static_cast<char>((crc >> 8) & 0xFF)); // High byte
        modbus.insert(0, static_cast<char>(0x00));
        modbus.insert(0, static_cast<char>(0x0A));
        modbus.push_back(static_cast<char>(0x0D));
        write(modbus);
    }
}

void BLEInterface::onCharacteristicChanged(
    const QLowEnergyCharacteristic& characteristic, 
    const QByteArray& value
)
{
    std::vector<int8_t> data(std::begin(value), std::end(value));

    std::cout << "Characteristic Changed: " << std::endl;
    if(value[0] == 0x0A) {
        auto tmp = value.mid(2, 3+value[3]);
        std::vector<uint8_t> dat(std::begin(tmp), std::end(tmp));
        for(auto&& el: dat) {
            std::cout << std::hex << +el << ' ';
        }

        std::cout << std::endl;
        std::vector<uint8_t> payload(dat.begin()+3, dat.begin()+7);
        std::rotate(std::begin(payload), std::begin(payload)+2, std::end(payload));
        float rvalue{};
        std::memcpy(&rvalue, std::vector<uint8_t>(payload.rbegin(), payload.rend()).data(), 4);
        for(auto&& el: payload) {
            std::cout << std::hex << +el << ' ';
        }
        std::cout << std::endl;
        std::cout << rvalue << std::endl;
        auto crc = Crc16(dat, dat.size());
        qDebug() << "CRC:" <<  QString::number(crc, 16);
        qDebug() <<  QString::number(crc, 16);
    }
}

void BLEInterface::onCharacteristicRead(const QLowEnergyCharacteristic &, const QByteArray& value)
{
    qDebug() << "Characteristic Read: " << value;

}

void BLEInterface::onCharacteristicWrite(const QLowEnergyCharacteristic &, const QByteArray &)
{
}

void BLEInterface::serviceError(QLowEnergyService::ServiceError)
{

}

void BLEInterface::read()
{
    if(modbus_service_ && readCharacteristic_.isValid())
        modbus_service_->readCharacteristic(readCharacteristic_);
}

void BLEInterface::searchCharacteristic(){
    if(modbus_service_){
        foreach (QLowEnergyCharacteristic c, modbus_service_->characteristics()) {
            if(c.isValid()){
                if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse ||
                    c.properties() & QLowEnergyCharacteristic::Write) {
                    writeCharacteristic_ = c;

                    if(c.properties() & QLowEnergyCharacteristic::WriteNoResponse)
                        writeMode_ = QLowEnergyService::WriteWithoutResponse;
                    else
                        writeMode_ = QLowEnergyService::WriteWithResponse;

                }
                if (c.properties() & QLowEnergyCharacteristic::Read) {
                    writeCharacteristic_ = c;
                    if(!readTimer_){
                        readTimer_ = new QTimer(this);
                        connect(readTimer_, &QTimer::timeout, this, &BLEInterface::read);
                        readTimer_->start(READ_INTERVAL_MS);
                    }
                }
                notificationDesc_ = c.descriptor(
                            QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                if (notificationDesc_.isValid()) {
                    modbus_service_->writeDescriptor(notificationDesc_, QByteArray::fromHex("0100"));
                }
            }
        }
    }
}

}
} //namespace ble