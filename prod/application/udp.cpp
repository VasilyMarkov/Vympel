#include "udp.hpp"
#include <iostream>

using namespace app;

namespace app {

UdpSocket::UdpSocket() {
    connect(&socket_, &QUdpSocket::readyRead, this, &UdpSocket::receivePortData);
}

void UdpSocket::sendPortData(const QByteArray& data)
{
    if(data.isEmpty()) return;

    socket_.writeDatagram(data, senderAddr_, senderPort_);
}

void UdpSocket::receivePortData()
{
    QByteArray datagram;
    datagram.resize(socket_.pendingDatagramSize());
    socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    emit sendData(QJsonDocument::fromJson(datagram, nullptr));
}

void UdpSocket::receiveData(const process_params_t& params) {

}

void UdpSocket::setSenderParameters(const QHostAddress& senderIp = QHostAddress::Any, quint16 senderPort = 1024)
{
    if (senderPort_ <= RESERVE_PORTS) throw std::runtime_error("Invalid sender port");

    senderAddr_ = senderIp;
    senderPort_ = senderPort;
}

void UdpSocket::setReceiverParameters(const QHostAddress& receiverIp = QHostAddress::Any, quint16 receiverPort = 1024)
{
    if (receiverPort <= RESERVE_PORTS) throw std::runtime_error("Invalid receiver port");

    socket_.bind(receiverIp, receiverPort);
}

}