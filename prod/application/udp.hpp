#ifndef UDP_H
#define UDP_H

#include <QUdpSocket>
#include <QJsonObject>
#include "interface.hpp"

namespace app {

constexpr int RESERVE_PORTS = 1024;

class UdpSocket final: public QObject {
    Q_OBJECT
public:
    UdpSocket();
    void sendPortData(const QByteArray&); //send to port
    void setSenderParameters(const QHostAddress&, quint16);
    void setReceiverParameters(const QHostAddress&, quint16);
private Q_SLOTS:
    void receivePortData();
public Q_SLOTS:
    void receiveData(const QJsonDocument&);
private:
    QUdpSocket socket_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
Q_SIGNALS:
    void sendData(const QJsonDocument&); 
};

}

#endif //UDP_H