#ifndef NETWORK_H
#define NETWORK_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonObject>
#include <QTimer>
#include <atomic>
#include <unordered_map>
#include "interface.hpp"

namespace app {

constexpr int RESERVE_PORTS = 1024;

class UdpHandler: public QObject {
    Q_OBJECT
public:
    UdpHandler() noexcept;
    void sendPortData(const QByteArray&); 
    void setSenderParameters(const QHostAddress&, quint16);
    void setReceiverParameters(const QHostAddress&, quint16);
private Q_SLOTS:
    virtual void receivePortData();
public Q_SLOTS:
    void receiveData(const QJsonDocument&);
protected:
    QUdpSocket udp_socket_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
Q_SIGNALS:
    void sendData(const QJsonDocument&); 
};

class CommandHandler: public UdpHandler {
    Q_OBJECT
public:
    CommandHandler() noexcept;
private:
    void receivePortData() override;
Q_SIGNALS:
    void setRateTemprature(double);
    void setCoreStatement(int);
    void closeApp();
};

class CameraConnector: public QObject {
    Q_OBJECT
public:
    CameraConnector();
private:
    QUdpSocket socket_;
    QTimer timer_;
};

class Network final: public QObject {
    Q_OBJECT
public:
    Network();
    QHostAddress getOwnIp();
    QHostAddress getHostIp() const;
private:
    void newTcpConnection();
Q_SIGNALS:
    void ready();
private Q_SLOTS:
    void handlingIncomingTcpPackets();
public Q_SLOTS:
    void receiveFuncCoefficients(const std::vector<double>&);
    void receiveCompressedImage(const std::vector<uint8_t>&);
private:
    std::unique_ptr<CameraConnector> cameraConnector_;
    QUdpSocket udpSocket_;
    QTcpServer tcpServer_;
    QTcpSocket* socket_;
    QHostAddress ownIp_;
    QHostAddress hostIp_;
};


}

#endif //NETWORK_H