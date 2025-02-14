#include <iostream>
#include <QtConcurrent>
#include "network.hpp"
#include "configReader.hpp"

namespace app {

UdpHandler::UdpHandler() noexcept {
    connect(&udp_socket_, &QUdpSocket::readyRead, this, &UdpHandler::receivePortData);
}

void UdpHandler::sendPortData(const QByteArray& data)
{
    if(data.isEmpty()) return;

    udp_socket_.writeDatagram(data, senderAddr_, senderPort_);
}

void UdpHandler::receivePortData()
{
    QByteArray datagram;
    datagram.resize(udp_socket_.pendingDatagramSize());
    udp_socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    emit sendData(QJsonDocument::fromJson(datagram, nullptr));

    auto jdoc = QJsonDocument::fromJson(datagram, nullptr);
}

void UdpHandler::receiveData(const QJsonDocument& json) {
    sendPortData(json.toJson());
}

void UdpHandler::setSenderParameters(const QHostAddress& senderIp = QHostAddress::Any, quint16 senderPort = 1024)
{
    if (senderPort <= RESERVE_PORTS) throw std::runtime_error("Invalid sender port");

    senderAddr_ = senderIp;
    senderPort_ = senderPort;
}

void UdpHandler::setReceiverParameters(const QHostAddress& receiverIp = QHostAddress::Any, quint16 receiverPort = 1024)
{
    if (receiverPort <= RESERVE_PORTS) throw std::runtime_error("Invalid receiver port");

    udp_socket_.bind(receiverIp, receiverPort);
}

CommandHandler::CommandHandler() noexcept {}

void CommandHandler::receivePortData()
{
    QByteArray datagram;
    datagram.resize(udp_socket_.pendingDatagramSize());
    udp_socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    // emit sendData(QJsonDocument::fromJson(datagram, nullptr));

    auto command = QJsonDocument::fromJson(datagram, nullptr)["commands"].toInt();
    switch(command) {
        case 1:
            Q_EMIT setCoreStatement(1);
        break;
        case 2:
            Q_EMIT setCoreStatement(2);
        break;
        case 3:
            Q_EMIT setRateTemprature(QJsonDocument::fromJson(datagram, nullptr)["tempratureRate"].toDouble());
        break;
        default:
        break;
    }
}

TcpHandler::TcpHandler() {
    connect(&tcp_server_, &QTcpServer::newConnection, this, &TcpHandler::onNewConnection);

    if (auto port = ConfigReader::getInstance().get("network", "cameraTcpPort").toInt(); tcp_server_.listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    } else {
        qDebug() << "Server failed to start:" << tcp_server_.errorString();
    }
}

void TcpHandler::onNewConnection() {
    QTcpSocket *socket = tcp_server_.nextPendingConnection();
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    QPointer<QTcpSocket> socketPtr(socket);

    QtConcurrent::run([this, socketPtr](){
        while(isOpenConnection) {
            QMetaObject::invokeMethod(this, [socketPtr]() {
                if (socketPtr) {
                    socketPtr->write("connected");
                    socketPtr->flush();
                }
            }, Qt::QueuedConnection);

            QThread::msleep(500);
        }
        return;
    });
}

TcpHandler::~TcpHandler() {
    isOpenConnection = false;
}

}