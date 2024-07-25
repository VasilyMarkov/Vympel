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
    app::Core core(argv[1]);
    QThread core_thread;
    core.moveToThread(&core_thread);
    core_thread.start();
    // quint16 port = 65001; // Specify the port to listen on
    // UdpReceiver receiver(port);
    

    core.attach(createUdpSocket(QHostAddress::LocalHost, app::constants::SENDER_PORT, 
                                QHostAddress::LocalHost, app::constants::RECEIVER_PORT));
    
    // core.process();
    return app.exec();
}