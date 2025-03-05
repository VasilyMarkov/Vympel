#include <iostream>
#include <algorithm>
#include <memory>
#include <QCoreApplication>
#include <QtConcurrent>
#include <QObject>
#include "bluetoothDevice.hpp"
#include "udp.hpp"
#include "utility.hpp"
#include "bleService.hpp"


int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);

    app::ble::BLEInterface bleDevice;
    auto [clientIp, clientPort] = app::parseJsonFile(
        "/home/vasily/usr/phystech/vympel/prod/conf/config.json").value();

    app::network::UdpSocket socket(QHostAddress(clientIp), clientPort);

    QThread thread;

    BlService blService;
    blService.moveToThread(&thread);
  
    QObject::connect(&thread, &QThread::started, &blService, &BlService::run, Qt::QueuedConnection);

    QObject::connect(&blService, &BlService::temperature, 
        &bleDevice, &app::ble::BLEInterface::temperature, Qt::QueuedConnection);

    QObject::connect(&bleDevice, &app::ble::BLEInterface::sendTemperature, 
        &socket, &app::network::UdpSocket::receiveData, Qt::QueuedConnection);

    thread.start();

    
    // QObject::connect(&timer, &QTimer::timeout, [&bleDevice]() 
    // {
    //     QtConcurrent::run([&bleDevice]()
    //     {
    //       bleDevice.temperature();
    //     });
    // });

    return app.exec();
  }

  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}