#ifndef UDP_H
#define UDP_H
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QJsonObject>
#include <qt6/QtCore/QJsonDocument>
#include "io_interface.hpp"


class UdpReceiver : public QObject
{
    Q_OBJECT

public:
    UdpReceiver(quint16 port)
    {
        socket = new QUdpSocket(this);
        connect(socket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);

        if (!socket->bind(QHostAddress::Any, port)) {
            qDebug() << "Failed to bind to port" << port;
            return;
        }

        qDebug() << "Listening on port" << port;
    }

private slots:
    void readPendingDatagrams()
    {
        while (socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            qDebug() << "Received message from" << sender.toString() << ":" << senderPort;
            qDebug() << "Message:" << datagram;
        }
    }

private:
    QUdpSocket *socket;
};


namespace app {

class UdpSocket final: public QObject, public ISender, public IReceiver, public IObserver {
    Q_OBJECT
public:
    explicit UdpSocket(const QHostAddress&, quint16);
    explicit UdpSocket(const QHostAddress&, quint16, const QHostAddress&, quint16);
public:
    void sendData(const QByteArray&) override;
    void update(const params_t&) override;
private slots:
    void receiveData() override;
private:
    QUdpSocket socket_;
    QHostAddress sender_addr_ = QHostAddress::LocalHost;
    QHostAddress receiver_addr_ = QHostAddress::LocalHost;
    quint16 sender_port_ = 1024;
    quint16 receiver_port_ = 1024;
private:
    QJsonObject json_;
};

}

#endif //UDP_H