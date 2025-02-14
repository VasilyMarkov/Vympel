#ifndef NETWORK_H
#define NETWORK_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonObject>
#include <atomic>
#include <unordered_map>
#include "interface.hpp"

namespace app {

constexpr int RESERVE_PORTS = 1024;

class TcpHandler final: public QObject {
    Q_OBJECT
public:
    TcpHandler();
    ~TcpHandler();
private:
    void onNewConnection(); 
private:
    std::unique_ptr<QTcpSocket> tcp_socket_;
    QTcpServer tcp_server_;
    std::atomic_bool isOpenConnection = true;
Q_SIGNALS:
    void sendData(const QJsonDocument&); 
};

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
};

}

#endif //NETWORK_H