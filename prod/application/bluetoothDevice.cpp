#include <iostream>
#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <unordered_set>
#include "bluetoothDevice.hpp"
#include "utility.hpp"
#include <bit>


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


QByteArray setTempratureSpeed(float rate, float brightness, float cond_mark, float vapor_mark, float half_sum_mark)
{
    static quint32 dbg = 0;
    QByteArray modbus_pdu;
    modbus_pdu.resize(32);

    // qDebug() << rate;
    static uint8_t cnt = 0;
    const uint16_t readTempratureRegisterAddress = 68;
    const uint16_t readTempratureRegisterNums = 2;
    // const uint16_t setTempratureRateRegisterAddress = 64;
    const uint16_t setTempratureRateRegisterAddress = 24;
    const uint16_t setTempratureRateRegisterNums = 2;
    brightness/= 1000.f;

    const auto rate_r = std::bit_cast<std::array<uint8_t, 4>>(rate);
    const auto brightness_r = std::bit_cast<std::array<uint8_t, 4>>(brightness);
    const auto cond_mark_r = std::bit_cast<std::array<uint8_t, 4>>(cond_mark);
    const auto vapor_mark_r = std::bit_cast<std::array<uint8_t, 4>>(vapor_mark);
    const auto half_sum_mark_r = std::bit_cast<std::array<uint8_t, 4>>(half_sum_mark);

    modbus_pdu[0] = cnt++;  //modbus address
    modbus_pdu[1] = 1;      //modbus address
    modbus_pdu[2] = 0x17;     //function code
    modbus_pdu[3] = static_cast<char>((readTempratureRegisterAddress >> 8) & 0xFF); 
    modbus_pdu[4] = static_cast<char>(readTempratureRegisterAddress & 0xFF); 
    modbus_pdu[5] = 0;
    modbus_pdu[6] = 2;
    modbus_pdu[7] = static_cast<char>((setTempratureRateRegisterAddress >> 8) & 0xFF); ;
    modbus_pdu[8] = static_cast<char>(setTempratureRateRegisterAddress & 0xFF); ;
    modbus_pdu[9] = 0;
    modbus_pdu[10] = 10;
    modbus_pdu[11] = sizeof(float) * 5;
    
    #if 1
    //reg 24-25 Скорость нагрева
    modbus_pdu[12] = rate_r[1];
    modbus_pdu[13] = rate_r[0];
    modbus_pdu[14] = rate_r[3];
    modbus_pdu[15] = rate_r[2]; 
    
    //reg 26-27 температура конденсации
    modbus_pdu[16] = brightness_r[1];
    modbus_pdu[17] = brightness_r[0];
    modbus_pdu[18] = brightness_r[3];
    modbus_pdu[19] = brightness_r[2]; 

    //reg 28-29 температура испарения
    modbus_pdu[20] = vapor_mark_r[1];
    modbus_pdu[21] = vapor_mark_r[0];
    modbus_pdu[22] = vapor_mark_r[3];
    modbus_pdu[23] = vapor_mark_r[2]; 

    //reg 30-31 температура точки росы
    modbus_pdu[24] = cond_mark_r[1];
    modbus_pdu[25] = cond_mark_r[0];
    modbus_pdu[26] = cond_mark_r[3];
    modbus_pdu[27] = cond_mark_r[2]; 

    //reg 32-33 значение функции обсчета видео для вывода на график
    modbus_pdu[28] = half_sum_mark_r[1];
    modbus_pdu[29] = half_sum_mark_r[0];
    modbus_pdu[30] = half_sum_mark_r[3];
    modbus_pdu[31] = half_sum_mark_r[2];
    #else
    
        //reg 24-25 Скорость нагрева
    modbus_pdu[12] = rate_r[1];
    modbus_pdu[13] = rate_r[0];
    modbus_pdu[14] = rate_r[3];
    modbus_pdu[15] = rate_r[2]; 
    
    //reg 26-27 температура конденсации
    modbus_pdu[16] = cond_mark_r[1];
    modbus_pdu[17] = cond_mark_r[0];
    modbus_pdu[18] = cond_mark_r[3];
    modbus_pdu[19] = cond_mark_r[2]; 

    //reg 28-29 температура испарения
    modbus_pdu[20] = vapor_mark_r[1];
    modbus_pdu[21] = vapor_mark_r[0];
    modbus_pdu[22] = vapor_mark_r[3];
    modbus_pdu[23] = vapor_mark_r[2]; 

    //reg 30-31 температура точки росы
    modbus_pdu[24] = half_sum_mark_r[1];
    modbus_pdu[25] = half_sum_mark_r[0];
    modbus_pdu[26] = half_sum_mark_r[3];
    modbus_pdu[27] = half_sum_mark_r[2]; 

    //reg 32-33 значение функции обсчета видео для вывода на график
    modbus_pdu[28] = brightness_r[1];
    modbus_pdu[29] = brightness_r[0];
    modbus_pdu[30] = brightness_r[3];
    modbus_pdu[31] = brightness_r[2]; 

   #endif 

    std::vector<uint8_t> v_data(std::begin(modbus_pdu)+1, std::end(modbus_pdu));

    auto crc = crc16(v_data, v_data.size());

    modbus_pdu.push_back(static_cast<char>(crc & 0xFF));       // Low byte
    modbus_pdu.push_back(static_cast<char>((crc >> 8) & 0xFF)); // High byte

    modbus_pdu = modify(modbus_pdu);

    modbus_pdu.insert(0, static_cast<char>(0x0A));
    modbus_pdu.push_back(static_cast<char>(0x0D));
    // qDebug() << "Output:" << byteArrayToHexString(modbus_pdu);
    return modbus_pdu;
}



QByteArray setTempratureSpeedOld(float value)
{
    QByteArray modbus_pdu;
    modbus_pdu.resize(12);

    static uint8_t cnt = 0;
    uint16_t first_register_address = 64;
    uint16_t registers_size = 2;

    auto bytes = std::bit_cast<std::array<uint8_t, 4>>(value);

    modbus_pdu[0] = cnt++; //modbus address
    modbus_pdu[1] = 1; //modbus address
    modbus_pdu[2] = 16; //function code
    modbus_pdu[3] = static_cast<char>((first_register_address >> 8) & 0xFF); 
    modbus_pdu[4] = static_cast<char>(first_register_address & 0xFF); 
    modbus_pdu[5] = 0;
    modbus_pdu[6] = registers_size;
    modbus_pdu[7] = sizeof(float);
    modbus_pdu[8] = bytes[1];
    modbus_pdu[9] = bytes[0];
    modbus_pdu[10] = bytes[3];
    modbus_pdu[11] = bytes[2]; 

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
        auto DEVICE_NAME = ConfigReader::getInstance().get("bluetooth", "deviceName").toString();
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
    if (value.isEmpty()) {
        std::cout << "value empty" << std::endl;
        return;
    }   
    // qDebug() << "Input:" << byteArrayToHexString(value);
    if(value[0] == 0x0A && value.size() == 12) {

        // if(changeRateTempratureIsSended_) {
            
        //     msgTimer_.stop();
        // }

        //show_packet(value, "[BT-in-value");
        auto modbus_reply = value.mid(1, value.size() - 2);
        int N = modbus_reply.size();
        for (int i=0; i < N; i++) {
            if (modbus_reply[i] == 0x10) {
                modbus_reply.remove(i, 1);
                N--;
                if (i < N) modbus_reply[i] = 0xFF - modbus_reply[i];
            }
        }
        modbus_reply.remove(0,1);

        //modbus_reply.replace("\x10\xEF", 2, "\x10", 1);
        //modbus_reply.replace("\x10\xF2", 2, "\x0D", 1);
        //modbus_reply.replace("\x10\xF5", 2, "\x0A", 1);
        //show_packet(modbus_reply, "[BT--destaff]");
        bool is_modbus_reply_ok = false;
        {
            std::vector<uint8_t> rx(std::begin(modbus_reply), std::end(modbus_reply));
            uint16_t crc = crc16(rx, rx.size() - 2);
            uint16_t crc_orig = modbus_reply[modbus_reply.size()-2] | (modbus_reply[modbus_reply.size()-1] << 8);
            if (crc == crc_orig) { 
                is_modbus_reply_ok = true;
                //std::cout << "CRC correct " << std::endl;
            } else {
                //std::cout << "CRC in packet: "<< std::hex << crc_orig;
                //std::cout << "crc calculated: " << std::hex << crc;
                //std::cout << std::endl;
            }
        }
        //packet specific
        if (is_modbus_reply_ok && (modbus_reply[0] == 1) && (modbus_reply[1] == 0x17) && (modbus_reply[2] == 4)) {
            union {
                float f;
                uint8_t c[4];
            } u;
            u.c[0] = modbus_reply[4]; u.c[1] = modbus_reply[3]; u.c[2] = modbus_reply[6]; u.c[3] = modbus_reply[5];
            // std::cout << "T = " << u.f << std::endl;

            Q_EMIT sendTemperature(u.f);
        }
        // auto tmp = value.mid(2, 3+value[3]);
        // std::vector<uint8_t> dat(std::begin(tmp), std::end(tmp));

        // std::vector<uint8_t> payload(dat.begin()+3, dat.begin()+7);
        // std::rotate(std::begin(payload), std::begin(payload)+2, std::end(payload));
        // float rvalue{};
        // uint32_t uintvalue{};
        // std::memcpy(&rvalue, std::vector<uint8_t>(payload.rbegin(), payload.rend()).data(), 4);

        // auto crc = crc16(dat, dat.size());
        // crc = (crc >> 8) | (crc << 8);
        
    
        // auto msb_crc = value[value.size()-3];
        // uint8_t lsb_crc = value[value.size()-2];
        // uint16_t value_crc = (msb_crc << 8) + lsb_crc;
        
        // if(crc == value_crc) {
        //     qDebug() << rvalue;
        //     Q_EMIT sendTemperature(rvalue);
        // }
        // Q_EMIT sendTemperature(rvalue);
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

void BLEInterface::changeRateTemprature(float rate, float brightness, float cond_mark, float vapor_mark, float half_sum_mark) {
    writeDataToCharachteristic(setTempratureSpeed(rate, brightness, cond_mark, vapor_mark, half_sum_mark));
    // qDebug() << "Temprature rate: " << rate;
}

void BLEInterface::temperature() {
    // writeDataToCharachteristic(createModbusPacket(TEMPERATURE_REGISTER, 2));
}

BLEInterface::~BLEInterface()
{
    low_energy_controller_->disconnectFromDevice();
    low_energy_controller_->deleteLater();
    deviceDiscoveryAgent_->stop();
    deviceDiscoveryAgent_->deleteLater();
    
}


} //namespace ble

} //namespace app