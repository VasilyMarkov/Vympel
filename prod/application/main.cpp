#include <iostream>
#include <algorithm>
#include <memory>
#include <QCoreApplication>
#include "app.hpp"

int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);

    app::Application application(app);
    // app::UdpSocket socket(QHostAddress::LocalHost, 64000);

    return app.exec();
  }

  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}

// #include <QCoreApplication>
// #include <QUdpSocket>
// #include <QDebug>
// #include <iostream>

// class UdpReceiver : public QObject {
//     Q_OBJECT

// public:
//     UdpReceiver(quint16 port) {
//         udpSocket = new QUdpSocket(this);
//         udpSocket->bind(QHostAddress::Any, port);

//         connect(udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::processPendingDatagrams);
//     }

// private slots:
//     void processPendingDatagrams() {
//         while (udpSocket->hasPendingDatagrams()) {
//             QByteArray datagram;
//             datagram.resize(udpSocket->pendingDatagramSize());
//             QHostAddress sender;
//             quint16 senderPort;

//             udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

//             qDebug() << "Message from:" << sender.toString();
//             qDebug() << "Message port:" << senderPort;
//             qDebug() << "Message:" << datagram;
//         }
//         std::cout << "receive" << std::endl;
//     }

// private:
//     QUdpSocket *udpSocket;
// };

// int main(int argc, char *argv[]) {
//     QCoreApplication a(argc, argv);

//     UdpReceiver receiver(65000); // Replace 12345 with your desired port

//     return a.exec();
// }

// #include "main.moc"