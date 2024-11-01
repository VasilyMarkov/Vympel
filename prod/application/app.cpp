#include "app.hpp"

app::Application::Application(const QCoreApplication& q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::core_mode_t>();

    auto configuration = app::parseJsonFile(
        "/home/vympel/usr/cv_project/prod/conf/config.json").value();
    socket_ = std::make_unique<UdpSocket>(QHostAddress(configuration["clientIp"].toString()), configuration["clientPort"].toInt(), 
        QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    bl_socket_ = std::make_unique<UdpSocket>(QHostAddress::LocalHost, 64000);

    // bluetoothDevice_ = std::make_unique<ble::BLEInterface>();

    std::string filename = "/home/vasily/usr/phystech/vympel/prod/application/video.mp4";
    core_ = std::make_unique<Core>(std::make_shared<app::CVision>(filename));

    core_->moveToThread(&core_thread_);

    connect(&core_thread_, &QThread::started, 
        core_.get(), &app::Core::process, Qt::QueuedConnection);
        
    // connect(bluetoothDevice_.get(), &ble::BLEInterface::deviceConnected, 
    //     core_.get(), &app::Core::process, Qt::QueuedConnection);

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

    // connect(core_.get(), &app::Core::requestTemperature, 
    //     bluetoothDevice_.get(), &ble::BLEInterface::requestTemperature, 
    //     Qt::QueuedConnection);

    connect(bl_socket_.get(), &app::UdpSocket::sendTemperature, 
        core_.get(), &app::Core::receiveTemperature, Qt::QueuedConnection);

    // connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
    //     core_.get(), &app::Core::receiveTemperature);
        
    core_thread_.start();
}