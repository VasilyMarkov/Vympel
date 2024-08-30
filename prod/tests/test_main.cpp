#include <iostream>
#include <algorithm>
#include <memory>
#include <qt5/QtNetwork/QUdpSocket>
#include <qt5/QtCore/QCoreApplication>
#include <qt5/QtCore/QThread>
#include "core.hpp"
#include "udp.hpp"
#include "processUnit.hpp"
#include "utility.hpp"

int main(int argc, char *argv[])
{
  try {
    QCoreApplication app(argc, argv);

    auto [clientIp, clientPort] = app::parseJsonFile("/home/vympel/usr/cv_project/prod/conf/config.json").value();
    std::cout << clientIp.toStdString() << ": " << clientPort << std::endl;
    app::UdpSocket socket(QHostAddress(clientIp), clientPort, 
                          QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    app::Core core(std::make_shared<test::ProcessUnit>());
    QThread thread;
    core.moveToThread(&thread);

    qRegisterMetaType<app::process_params_t>();
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