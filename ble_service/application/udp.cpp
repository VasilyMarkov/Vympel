#include "udp.hpp"
#include <iostream>
#include <QVariant>
namespace app
{
namespace network
{

UdpSocket::UdpSocket(const QHostAddress& sender_addr, quint16 sender_port):     
    sender_addr_(sender_addr), 
    sender_port_(sender_port) 
{
    if (sender_port_ <= RESERVE_PORTS) throw std::runtime_error("Invalid sender port");
        // socket_.bind(QHostAddress(QString("192.168.32.192")), 65000);
        socket_.bind(QHostAddress(QString("192.168.234.192")), 65000);
    // socket_.bind(QHostAddress(QString("127.0.0.1")), 64000);
}

UdpSocket::UdpSocket
(
    const QHostAddress& sender_addr, 
    quint16 sender_port, 
    const QHostAddress& receiver_addr, 
    quint16 receiver_port
): 
    sender_addr_(sender_addr), 
    sender_port_(sender_port), 
    receiver_addr_(receiver_addr), 
    receiver_port_(receiver_port) 
{
    if (sender_port_ <= RESERVE_PORTS) throw std::runtime_error("Invalid sender port");
    if (receiver_port_ <= RESERVE_PORTS) throw std::runtime_error("Invalid receiver port");

    socket_.bind(QHostAddress(QString("192.168.234.192")), 64000);
    // socket_.bind(QHostAddress(QString("127.0.0.1")), 64000);
    connect(&socket_, &QUdpSocket::readyRead, this, &UdpSocket::receivePortData);
}

void UdpSocket::sendPortData(const QByteArray& data)
{
    if(data.isEmpty()) return;

    socket_.writeDatagram(data, QHostAddress(QString("192.168.234.192")), 64000);
}

void UdpSocket::receivePortData()
{
    QByteArray datagram;
    datagram.resize(socket_.pendingDatagramSize());
    socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    auto json = QJsonDocument::fromJson(datagram, nullptr);
    auto mode = json.object().value("core_mode").toString();
    emit sendData(mode);
}

void UdpSocket::receiveData(double temperature) {
    QJsonObject json;
    json["temperature"] = temperature;
    std::cout << "send" << std::endl;
    sendPortData(QJsonDocument(json).toJson());
}

} //namespace network

} //namespace app