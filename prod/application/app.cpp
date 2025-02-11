#include "app.hpp"
#include "logger.hpp"
#include <QtConcurrent>

namespace app {

Application::Application(const QCoreApplication& q_core_app): q_core_app_(q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::EventType>();

    socket_ = std::make_unique<UdpSocket>();
    socket_->setReceiverParameters(QHostAddress(ConfigReader::getInstance().get("network", "cameraIp").toString()), 
                                   ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
    socket_->setSenderParameters(QHostAddress(ConfigReader::getInstance().get("network", "hostIp").toString()), 
                                   ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());
#ifndef NOT_BLE
    runBle();
#endif
    runCore();
}

void Application::runCore() {

#ifndef NOT_BLE
    if(!ble_thread_.isRunning()) {
        throw std::runtime_error("The core can only be started after the startup BLE");
    }
#endif
    core_ = std::make_unique<Core>(std::make_shared<app::NetProcessing>());
    core_->moveToThread(&core_thread_);

    connect(&core_thread_, &QThread::started, 
        core_.get(), &app::Core::process, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::exit, 
        &core_thread_, &QThread::quit, Qt::QueuedConnection);
    connect(&core_thread_, &QThread::finished, 
        core_.get(), &QObject::deleteLater, Qt::QueuedConnection);
    connect(&core_thread_, &QThread::finished,
        &q_core_app_, &QCoreApplication::quit, Qt::QueuedConnection);
    connect(socket_.get(), &app::UdpSocket::sendData, 
        core_.get(), &app::Core::receiveData, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::sendData, 
        socket_.get(), &app::UdpSocket::receiveData, Qt::QueuedConnection);

    core_thread_.start();
}

void Application::runBle() {
    bluetoothDevice_ = std::make_unique<ble::BLEInterface>();
    bluetoothDevice_->moveToThread(&ble_thread_);

    connect(&ble_thread_, &QThread::started, 
        bluetoothDevice_.get(), &ble::BLEInterface::run, Qt::QueuedConnection);
    connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
        core_.get(), &app::Core::receiveTemperature, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::requestTemperature, 
        bluetoothDevice_.get(), &ble::BLEInterface::temperature, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::requestSlowCooling, 
        bluetoothDevice_.get(), &ble::BLEInterface::slowCooling, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::requestSlowHeating, 
        bluetoothDevice_.get(), &ble::BLEInterface::slowHeating, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::requestFastCooling, 
        bluetoothDevice_.get(), &ble::BLEInterface::fastCooling, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::requestFastHeating, 
        bluetoothDevice_.get(), &ble::BLEInterface::fastHeating, Qt::QueuedConnection);
    connect(bluetoothDevice_.get(), &ble::BLEInterface::isReady,
        core_.get(), &app::Core::setBlEStatus, Qt::QueuedConnection);

    ble_thread_.start();
}

Application::~Application()
{

}

} //namespace app