#include <iostream>
#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QtConcurrent>
#include <unordered_set>
#include "bluetoothDevice.hpp"
#include "utility.hpp"

void print(const QByteArray& bytearray) {
    for(auto&& el : bytearray) {
        std::cout << static_cast<uint8_t>(el) << ' ';
    }
    std::cout << std::endl;
}

QString byteArrayToHexString(const QByteArray &byteArray) {
    QString hexString;
    for (char byte : byteArray) {
        if (!hexString.isEmpty()) {
            hexString.append(' ');
        }
        hexString.append(QString("%1").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')).toUpper());
    }
    return hexString;
}

namespace app 
{

QByteArray modify(const QByteArray& pdu) {
    static const std::unordered_set bytes = {0xA, 0xD, 0x10};
    QByteArray result;
    result.reserve(pdu.size());
    for(auto&& val : pdu) {
        if(bytes.contains(val)) {
            result.push_back(0x10);
            result.push_back(0xFF-val);    
        }
        else {
            result.push_back(val);
        }
    }
    return result;
}


QByteArray createModbusPacket(uint16_t first_register_address, uint16_t registers_amount)
{
    QByteArray modbus_pdu;
    modbus_pdu.resize(6);

    modbus_pdu[0] = 1; //modbus address
    modbus_pdu[1] = 4; //function code
    modbus_pdu[2] = static_cast<char>((first_register_address >> 8) & 0xFF); 
    modbus_pdu[3] = static_cast<char>(first_register_address & 0xFF); 
    modbus_pdu[4] = static_cast<char>((registers_amount >> 8) & 0xFF);
    modbus_pdu[5] = static_cast<char>(registers_amount & 0xFF);

    std::vector<uint8_t> v_data(std::begin(modbus_pdu), std::end(modbus_pdu));

    auto crc = crc16(v_data, v_data.size());

    modbus_pdu.push_back(static_cast<char>(crc & 0xFF));       // Low byte
    modbus_pdu.push_back(static_cast<char>((crc >> 8) & 0xFF)); // High byte
    modbus_pdu.insert(0, static_cast<char>(0x00));
    modbus_pdu.insert(0, static_cast<char>(0x0A));
    modbus_pdu.push_back(static_cast<char>(0x0D));
    
    return modbus_pdu;
}

QByteArray writeModbusRegister(uint16_t first_register_address, uint16_t value)
{
    QByteArray modbus_pdu;
    modbus_pdu.resize(10);
    
    static uint8_t cnt = 0;

    modbus_pdu[0] = cnt++; //modbus address
    modbus_pdu[1] = 1; //modbus address
    modbus_pdu[2] = 16; //function code
    modbus_pdu[3] = static_cast<char>((first_register_address >> 8) & 0xFF); 
    modbus_pdu[4] = static_cast<char>(first_register_address & 0xFF); 
    modbus_pdu[5] = 0;
    modbus_pdu[6] = 1;
    modbus_pdu[7] = 2;
    modbus_pdu[8] = 0;
    modbus_pdu[9] = 1;

    std::vector<uint8_t> v_data(std::begin(modbus_pdu)+1, std::end(modbus_pdu));

    auto crc = crc16(v_data, v_data.size());

    modbus_pdu.push_back(static_cast<char>(crc & 0xFF));       // Low byte
    modbus_pdu.push_back(static_cast<char>((crc >> 8) & 0xFF)); // High byte
    modbus_pdu = modify(modbus_pdu);
    modbus_pdu.insert(0, static_cast<char>(0x0A));
    modbus_pdu.push_back(static_cast<char>(0x0D));

    return modbus_pdu;
}

QByteArray setTempratureSpeed(float value)
{
    QByteArray modbus_pdu;
    modbus_pdu.resize(12);

    static uint8_t cnt = 0;
    uint16_t first_register_address = 64;
    uint16_t registers_size = 2;


    union {
        uint8_t c[4];
        float f;
    } u;
    u.f = value;

    modbus_pdu[0] = cnt++; //modbus address
    modbus_pdu[1] = 1; //modbus address
    modbus_pdu[2] = 16; //function code
    modbus_pdu[3] = static_cast<char>((first_register_address >> 8) & 0xFF); 
    modbus_pdu[4] = static_cast<char>(first_register_address & 0xFF); 
    modbus_pdu[5] = 0;
    modbus_pdu[6] = registers_size;
    modbus_pdu[7] = sizeof(float);
    modbus_pdu[8] = u.c[1];
    modbus_pdu[9] = u.c[0];
    modbus_pdu[10] = u.c[3];
    modbus_pdu[11] = u.c[2]; 

    std::vector<uint8_t> v_data(std::begin(modbus_pdu)+1, std::end(modbus_pdu));

    auto crc = crc16(v_data, v_data.size());

    modbus_pdu.push_back(static_cast<char>(crc & 0xFF));       // Low byte
    modbus_pdu.push_back(static_cast<char>((crc >> 8) & 0xFF)); // High byte

    modbus_pdu = modify(modbus_pdu);

    modbus_pdu.insert(0, static_cast<char>(0x0A));
    modbus_pdu.push_back(static_cast<char>(0x0D));

    return modbus_pdu;
}

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
    Q_EMIT deviceChanged();
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
}

void BLEInterface::run() {
    deviceDiscoveryAgent_->start();
}

void BLEInterface::writeDataToCharachteristic(const QByteArray& data)
{
    const QString RX_CHAR_UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    auto characteristic = modbus_service_->characteristic(QBluetoothUuid(RX_CHAR_UUID));
    if (characteristic.isValid()) {
        modbus_service_->writeCharacteristic(characteristic, data);
    }
}

void BLEInterface::requestTemperature() 
{
    QString RX_SERVICE_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
    auto characteristic = modbus_service_->characteristic(QBluetoothUuid(RX_SERVICE_UUID));
    if (characteristic.isValid()) {
        searchCharacteristic();
        writeDataToCharachteristic(createModbusPacket(68, 2));
    }
}

void BLEInterface::addDevice(const QBluetoothDeviceInfo& device) 
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        std::cout << device.name().toStdString() << ' ' << device.address().toString().toStdString() << std::endl;
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


        Q_EMIT deviceConnected();
        Q_EMIT isReady();
    }
}

void BLEInterface::onCharacteristicChanged(
    const QLowEnergyCharacteristic& characteristic, 
    const QByteArray& value
)
{
    // qDebug() << "Input:" << value;
    if (value.isEmpty()) {
        std::cout << "value empty" << std::endl;
        return;
    }   
    if(value[0] == 0x0A && value.size() == 12) {
        auto tmp = value.mid(2, 3+value[3]);
        std::vector<uint8_t> dat(std::begin(tmp), std::end(tmp));

        std::vector<uint8_t> payload(dat.begin()+3, dat.begin()+7);
        std::rotate(std::begin(payload), std::begin(payload)+2, std::end(payload));
        float rvalue{};
        uint32_t uintvalue{};
        std::memcpy(&rvalue, std::vector<uint8_t>(payload.rbegin(), payload.rend()).data(), 4);

        auto crc = crc16(dat, dat.size());
        crc = (crc >> 8) | (crc << 8);
        
    
        auto msb_crc = value[value.size()-3];
        uint8_t lsb_crc = value[value.size()-2];
        uint16_t value_crc = (msb_crc << 8) + lsb_crc;

        if(crc == value_crc) {
            Q_EMIT sendTemperature(rvalue);
        }
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

void BLEInterface::changeRateTemprature(double rate) {
    writeDataToCharachteristic(setTempratureSpeed(rate));
    std::cout << rate << std::endl;
    // writeDataToCharachteristic(QByteArray());
}

void BLEInterface::temperature() {
    writeDataToCharachteristic(createModbusPacket(TEMPERATURE_REGISTER, 2));
}

} //namespace ble

} //namespace app