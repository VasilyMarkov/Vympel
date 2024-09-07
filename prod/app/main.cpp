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

int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);
    auto [clientIp, clientPort] = app::parseJsonFile("/home/vasily/usr/phystech/vympel/prod/conf/config.json").value();
    app::UdpSocket socket(QHostAddress(clientIp), clientPort, 
                          QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    // app::DeviceDiscovery blDevice;
    // blDevice.startScan();
    app::ble::BLEInterface ble;
    ble.scanDevices();
    // bl.connectToDevice();
    // std::string filename = "/home/vasily/usr/phystech/vympel/prod/app/video.mp4";
    // app::Core core(std::make_shared<app::CVision>(filename));
    // QThread thread;
    // core.moveToThread(&thread);

    // qRegisterMetaType<app::process_params_t>();
    // qRegisterMetaType<app::core_mode_t>();

    // QObject::connect(&thread, &QThread::started, &core, &app::Core::process, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::exit, &thread, &QThread::quit, Qt::QueuedConnection);
    // QObject::connect(&thread, &QThread::finished, &core, &QObject::deleteLater, Qt::QueuedConnection);
    // QObject::connect(&thread, &QThread::finished, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    // QObject::connect(&socket, &app::UdpSocket::sendData, &core, &app::Core::receiveData, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::sendData, &socket, &app::UdpSocket::receiveData, Qt::QueuedConnection);

    // thread.start();

    return app.exec();
  }

  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}