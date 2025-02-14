#include "app.hpp"
#include "logger.hpp"
#include <QtConcurrent>

namespace app {

Application::Application(const QCoreApplication& q_core_app): q_core_app_(q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::EventType>();

    udp_handler_ = std::make_unique<CommandHandler>();
    udp_handler_->setReceiverParameters(QHostAddress(ConfigReader::getInstance().get("network", "cameraIp").toString()), 
                                   ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
    udp_handler_->setSenderParameters(QHostAddress(ConfigReader::getInstance().get("network", "hostIp").toString()), 
                                   ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());

    tcp_handler_ = std::make_unique<TcpHandler>();
#ifndef NOT_BLE
    runBle();
#endif
    runCore();

    auto camera_python_process_path = fs::current_path().parent_path() / 
        ConfigReader::getInstance().get("files", "camera_python_script").toString().toStdString();
    QStringList args = QStringList() << QString::fromStdString(camera_python_process_path.string());
    camera_python_.start ("python3", args);

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

    connect(udp_handler_.get(), &app::CommandHandler::setCoreStatement, 
        core_.get(), &app::Core::setCoreStatement, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::sendData, 
        udp_handler_.get(), &app::UdpHandler::receiveData, Qt::QueuedConnection);

    connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
        core_.get(), &app::Core::receiveTemperature, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::requestTemperature, 
        bluetoothDevice_.get(), &ble::BLEInterface::temperature, Qt::QueuedConnection);

    connect(bluetoothDevice_.get(), &ble::BLEInterface::isReady,
        core_.get(), &app::Core::setBlEStatus, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::setRateTemprature, 
        bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);

    core_thread_.start();
}

void Application::runBle() {
    bluetoothDevice_ = std::make_unique<ble::BLEInterface>();
    bluetoothDevice_->moveToThread(&ble_thread_);

    connect(&ble_thread_, &QThread::started, 
        bluetoothDevice_.get(), &ble::BLEInterface::run, Qt::QueuedConnection);

    connect(udp_handler_.get(), &app::CommandHandler::setRateTemprature, 
        bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);

    ble_thread_.start();
}

Application::~Application()
{

}

} //namespace app