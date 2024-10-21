#include "app.hpp"

app::Application::Application(const QCoreApplication& q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::core_mode_t>();

    auto [clientIp, clientPort] = app::parseJsonFile("/home/vasily/usr/phystech/vympel/prod/conf/config.json").value();
    socket_ = std::make_unique<UdpSocket>(QHostAddress(clientIp), clientPort, 
        QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    std::string filename = "/home/vasily/usr/phystech/vympel/prod/app/video.mp4";
    core_ = std::make_unique<Core>(std::make_shared<app::CVision>(filename));

    core_->moveToThread(&core_thread_);

    QObject::connect(&core_thread_, &QThread::started, core_.get(), &app::Core::process, Qt::QueuedConnection);
    QObject::connect(core_.get(), &app::Core::exit, &core_thread_, &QThread::quit, Qt::QueuedConnection);
    QObject::connect(&core_thread_, &QThread::finished, core_.get(), &QObject::deleteLater, Qt::QueuedConnection);
    QObject::connect(&core_thread_, &QThread::finished, &q_core_app, &QCoreApplication::quit, Qt::QueuedConnection);
    QObject::connect(socket_.get(), &app::UdpSocket::sendData, core_.get(), &app::Core::receiveData, Qt::QueuedConnection);
    QObject::connect(core_.get(), &app::Core::sendData, socket_.get(), &app::UdpSocket::receiveData, Qt::QueuedConnection);

    core_thread_.start();
}