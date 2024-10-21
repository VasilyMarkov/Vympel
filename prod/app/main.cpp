#include <iostream>
#include <algorithm>
#include <memory>
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QCoreApplication>
#include <qt6/QtCore/QThread>
#include "core.hpp"
#include "udp.hpp"
#include "cv.hpp"
#include "utility.hpp"
#include "bluetoothDevice.hpp"
#include "app.hpp"

int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);
    app::Application application(app);
    // auto [clientIp, clientPort] = app::parseJsonFile("/home/vasily/usr/phystech/vympel/prod/conf/config.json").value();
    // app::UdpSocket socket(QHostAddress(clientIp), clientPort, 
    //                       QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    // app::ble::BLEInterface bluetoothDevice;
    
    // std::string filename = "/home/vasily/usr/phystech/vympel/prod/app/video.mp4";
    // app::Core core(std::make_shared<app::CVision>(filename));

    // QThread core_thread;
    // core.moveToThread(&core_thread);

    // qRegisterMetaType<app::process_params_t>();
    // qRegisterMetaType<app::core_mode_t>();

    // QObject::connect(&core_thread, &QThread::started, &core, &app::Core::process, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::exit, &core_thread, &QThread::quit, Qt::QueuedConnection);
    // QObject::connect(&core_thread, &QThread::finished, &core, &QObject::deleteLater, Qt::QueuedConnection);
    // QObject::connect(&core_thread, &QThread::finished, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    // QObject::connect(&socket, &app::UdpSocket::sendData, &core, &app::Core::receiveData, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::sendData, &socket, &app::UdpSocket::receiveData, Qt::QueuedConnection);

    // core_thread.start();

    return app.exec();
  }

  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}