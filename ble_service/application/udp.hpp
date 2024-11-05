#ifndef UDP_H
#define UDP_H

#include <QUdpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "interface.hpp"

namespace app {

namespace network
{
constexpr int RESERVE_PORTS = 1024;
    
class UdpSocket final: public QObject {
    Q_OBJECT
public:
    explicit UdpSocket(const QHostAddress&, quint16);
    explicit UdpSocket(const QHostAddress&, quint16, const QHostAddress&, quint16);
public:
    void sendPortData(const QByteArray&); //send to port
private slots:
    void receivePortData();
public slots:
    void receiveData(double);
private:
    QUdpSocket socket_;
    QHostAddress sender_addr_ = QHostAddress::LocalHost;
    QHostAddress receiver_addr_ = QHostAddress::LocalHost;
    quint16 sender_port_;
    quint16 receiver_port_;
private:
    QJsonObject json_;
    signals:
    void sendData(const QString&); //send to core
};


} // namespace network

} // namespace app

#endif //UDP_H