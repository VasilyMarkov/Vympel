#include <iostream>
#include <algorithm>
#include <memory>
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QCoreApplication>
#include <qt6/QtCore/QThread>
#include "core.hpp"
#include "udp.hpp"


std::unique_ptr<app::UdpSocket> createUdpSocket(const QHostAddress& sender_addr, quint16 sender_port) 
{
  return std::make_unique<app::UdpSocket>(sender_addr, sender_port);
}

std::unique_ptr<app::UdpSocket> createUdpSocket(const QHostAddress& sender_addr, quint16 sender_port, 
                                                const QHostAddress& receiver_addr, quint16 receiver_port) 
{
  return std::make_unique<app::UdpSocket>(sender_addr, sender_port, receiver_addr, receiver_port);
}


// int main(int argc, char** argv )
// {
//   if ( argc != 2 )
//   {
//     printf("usage: DisplayImage.out <Image_Path>\n");
    
//     return -1;
//   }
//   // auto socket = createUdpSocket(QHostAddress::LocalHost, app::constants::SENDER_PORT, 
//   //                               QHostAddress::LocalHost, app::constants::RECEIVER_PORT);
//   // char s = 's';
//   // // socket->sendData(QByteArray(5, s));
//   // while(true) {
//   //   // socket->sendData(QByteArray(5, s));
//   // }

//   try {
//     app::Core core(argv[1]);

//     core.attach(createUdpSocket(QHostAddress::LocalHost, app::constants::SENDER_PORT, 
//                                 QHostAddress::LocalHost, app::constants::RECEIVER_PORT));
//     core.process();
//   }
//   catch (const std::exception& e) {
//     std::cout << e.what() << std::endl;
//   }
// }

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app::UdpSocket socket(QHostAddress::LocalHost, app::constants::SENDER_PORT, 
                          QHostAddress::LocalHost, app::constants::RECEIVER_PORT);

    app::Core core(argv[1]);
    QThread thread;
    core.moveToThread(&thread);
    qRegisterMetaType<app::params_t>();
    qRegisterMetaType<app::core_mode_t>();
    QObject::connect(&thread, &QThread::started, &core, &app::Core::process);
    QObject::connect(&thread, &QThread::finished, &core, &QObject::deleteLater);
    QObject::connect(&socket, &app::UdpSocket::sendData, &core, &app::Core::receiveData, Qt::QueuedConnection);
    QObject::connect(&core, &app::Core::sendData, &socket, &app::UdpSocket::receiveData, Qt::QueuedConnection);

    // core.attach(createUdpSocket(QHostAddress::LocalHost, app::constants::SENDER_PORT, 
    //                             QHostAddress::LocalHost, app::constants::RECEIVER_PORT));

    thread.start();

    return app.exec();
}