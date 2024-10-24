#include "udp.hpp"
#include <iostream>

using namespace app;

UdpSocket::UdpSocket(const QHostAddress& sender_addr, quint16 sender_port):     
    sender_addr_(sender_addr), 
    sender_port_(sender_port) 
{
    if (sender_port_ < 1024) throw std::runtime_error("Invalid sender port");
    socket_ = new QUdpSocket(this);
    // socket_.bind(QHostAddress::LocalHost, 64000);
    if (!socket_->bind(QHostAddress::Any, 64000)) {
        qDebug() << "Failed to bind to port" << 64000;
        return;
    }
    // std::cout << sender_addr_.toString().toStdString() <<  ' ' << 65000 << std::endl;

    connect(socket_, &QUdpSocket::readyRead, this, &UdpSocket::receiveTemperature);
    // connect(socket_, &QUdpSocket::readyRead, [](){std::cout << "receive" << std::endl;});
}

UdpSocket::UdpSocket(
    const QHostAddress& sender_addr, quint16 sender_port, const QHostAddress& receiver_addr, quint16 receiver_port): 
    sender_addr_(sender_addr), sender_port_(sender_port), receiver_addr_(receiver_addr), receiver_port_(receiver_port) 
{
    if (sender_port_ < 1024) throw std::runtime_error("Invalid sender port");
    if (receiver_port_ < 1024) throw std::runtime_error("Invalid receiver port");

    socket_->bind(sender_addr_, sender_port_);
    connect(socket_, &QUdpSocket::readyRead, this, &UdpSocket::receivePortData);
    // connect(&socket_, &QUdpSocket::readyRead, this, &UdpSocket::receiveTemperature);
}

void UdpSocket::sendPortData(const QByteArray& data)
{
    if(data.isEmpty()) return;
    socket_->writeDatagram(data, sender_addr_, sender_port_);
}

void UdpSocket::receivePortData()
{
    QByteArray datagram;
    datagram.resize(socket_->pendingDatagramSize());
    socket_->readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    auto json = QJsonDocument::fromJson(datagram, nullptr);
    auto mode = json.object().value("core_mode").toString();
    Q_EMIT sendData(mode);
}

void UdpSocket::receiveTemperature()
{
    // QByteArray datagram;
    // datagram.resize(socket_.pendingDatagramSize());
    // socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    // auto json = QJsonDocument::fromJson(datagram, nullptr);
    // auto mode = json.object().value("temperature").toDouble();

    while (socket_->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket_->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            socket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

            auto json = QJsonDocument::fromJson(datagram, nullptr);
            auto temp = json.object().value("temperature").toDouble();
            std::cout << temp << std::endl;
        }

    // std::cout << "receive" << std::endl;
    // Q_EMIT sendTemperature(mode);
}

void UdpSocket::receiveData(const process_params_t& params) {
    json_["brightness"] = params.brightness;
    json_["filtered"] = params.filtered;
    sendPortData(QJsonDocument(json_).toJson());
}
