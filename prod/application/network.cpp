#include <iostream>
#include <QtConcurrent>
#include <QNetworkInterface>
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

CameraDiscoverHandler::CameraDiscoverHandler(const QHostAddress& ownIp): 
    ownIp_(ownIp), 
    port_(ConfigReader::getInstance().get("network", "cameraDiscoverPort").toInt())  
{
    QObject::connect(&timer_, &QTimer::timeout, this, &CameraDiscoverHandler::sendBroadcast);
    timer_.start(2000);
}

void CameraDiscoverHandler::sendBroadcast() {
    QByteArray datagram = ownIp_.toString().toUtf8();
    udp_socket_.writeDatagram({}, QHostAddress::Broadcast, port_);
}

TcpHandler::TcpHandler() {
    connect(&tcp_server_, &QTcpServer::newConnection, this, &TcpHandler::onNewConnection);

    if (auto port =  ConfigReader::getInstance().get("network", "cameraTcpPort").toInt(); tcp_server_.listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    } else {
        qDebug() << "Server failed to start:" << tcp_server_.errorString();
    }
}

void TcpHandler::onNewConnection() {
    QTcpSocket *socket = tcp_server_.nextPendingConnection();
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::readyRead, this, &TcpHandler::handlingIncomingTcpPackets);

    QString clientInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
    qDebug() << "New client connected from:" << clientInfo;
    hostIp_ = socket->peerAddress();
    emit closeCameraDiscoverHandler();
}

void TcpHandler::handlingIncomingTcpPackets() {
    // auto message = QString(tcp_socket_->readAll());
    // qDebug() << message;
}

TcpHandler::~TcpHandler() {
    isOpenConnection = false;
}


CameraConnector::CameraConnector()
{
    connect(&timer_, &QTimer::timeout, [this](){
        socket_.writeDatagram({}, QHostAddress::Broadcast, ConfigReader::getInstance().get("network", "cameraDiscoverPort").toInt());
    });
    timer_.start(2000);
}

Network::Network():
    ownIp_(getOwnIp())
{
    connect(&tcpServer_, &QTcpServer::newConnection, this, &Network::newTcpConnection);

    cameraConnector_ = std::make_unique<CameraConnector>();

    if (auto port =  ConfigReader::getInstance().get("network", "cameraTcpPort").toInt(); tcpServer_.listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    } else {
        qDebug() << "Server failed to start:" << tcpServer_.errorString();
    }   
}

void Network::newTcpConnection() {
    QTcpSocket *socket = tcpServer_.nextPendingConnection();
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::readyRead, this, &Network::handlingIncomingTcpPackets);

    QString clientInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
    qDebug() << "New client connected from:" << clientInfo;
    hostIp_ = socket->peerAddress();
    
    cameraConnector_.reset();
}

QHostAddress Network::getOwnIp()
{
    auto interfaces = QNetworkInterface::allInterfaces();
    for(auto&& interface : interfaces) {
        if(interface.flags().testFlag(QNetworkInterface::IsUp)) {
            for(auto&& entry: interface.addressEntries()) {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback()) {
                    return entry.ip();
                }
            }
        }
    }
    return QHostAddress(ConfigReader::getInstance().get("network", "cameraIp").toString());
}

void Network::handlingIncomingTcpPackets() {

}

}