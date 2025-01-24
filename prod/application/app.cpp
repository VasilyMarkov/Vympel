#include "app.hpp"
#include "logger.hpp"

app::Application::Application(const QCoreApplication& q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::EventType>();

    socket_ = std::make_unique<UdpSocket>();
    socket_->setReceiverParameters(QHostAddress(configReader.get("network", "clientIp").toString()), 
                                   configReader.get("network", "controlFromServiceProgramPort").toInt());
    socket_->setSenderParameters(QHostAddress(configReader.get("network", "clientIp").toString()), 
                                   configReader.get("network", "serviceProgramPort").toInt());
    
    
    core_ = std::make_unique<Core>(std::make_shared<app::CameraProcessingModule>());

    core_->moveToThread(&core_thread_);

    connect(&core_thread_, &QThread::started, 
        core_.get(), &app::Core::process, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::exit, 
        &core_thread_, &QThread::quit, Qt::QueuedConnection);

    connect(&core_thread_, &QThread::finished, 
        core_.get(), &QObject::deleteLater, Qt::QueuedConnection);

    connect(&core_thread_, &QThread::finished,
        &q_core_app, &QCoreApplication::quit, Qt::QueuedConnection);

    connect(socket_.get(), &app::UdpSocket::sendData, 
        core_.get(), &app::Core::receiveData, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::sendData, 
        socket_.get(), &app::UdpSocket::receiveData, Qt::QueuedConnection);


    // bluetoothDevice_ = std::make_unique<ble::BLEInterface>();

    // connect(bluetoothDevice_.get(), &ble::BLEInterface::deviceConnected, 
    //     core_.get(), &app::Core::process, Qt::QueuedConnection);
        
    // connect(core_.get(), &app::Core::requestTemperature, 
    //     bluetoothDevice_.get(), &ble::BLEInterface::temperature, Qt::QueuedConnection);

    // connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
    //     core_.get(), &app::Core::receiveTemperature);


    core_thread_.start();
}

app::Application::~Application()
{
    // logger.destroyLog();
}
