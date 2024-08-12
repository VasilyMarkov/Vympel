#include <iostream>
#include <algorithm>
#include <memory>
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QCoreApplication>
#include <qt6/QtCore/QThread>
#include "core.hpp"
#include "udp.hpp"

int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);

    app::UdpSocket socket(QHostAddress::LocalHost, app::constants::port::SENDER_PORT, 
                          QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);
    // app::Core core(argv[1]);
    // app::Core core("/home/vasily/usr/phystech/vympel/prod/video.mp4");
    app::Core core("/home/vasily/usr/phystech/vympel/prod/video.mp4");
    QThread thread;
    core.moveToThread(&thread);

    qRegisterMetaType<app::cv_params_t>();
    qRegisterMetaType<app::core_mode_t>();

    QObject::connect(&thread, &QThread::started, &core, &app::Core::process, Qt::QueuedConnection);
    QObject::connect(&core, &app::Core::exit, &thread, &QThread::quit, Qt::QueuedConnection);
    QObject::connect(&thread, &QThread::finished, &core, &QObject::deleteLater, Qt::QueuedConnection);
    QObject::connect(&thread, &QThread::finished, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    QObject::connect(&socket, &app::UdpSocket::sendData, &core, &app::Core::receiveData, Qt::QueuedConnection);
    QObject::connect(&core, &app::Core::sendData, &socket, &app::UdpSocket::receiveData, Qt::QueuedConnection);

    thread.start();

    return app.exec();
  }
  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}