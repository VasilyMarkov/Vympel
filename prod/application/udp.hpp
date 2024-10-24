#ifndef UDP_H
#define UDP_H

#include <QUdpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "interface.hpp"

namespace app {

class UdpSocket final: public QObject {
    Q_OBJECT
public:
    explicit UdpSocket(const QHostAddress&, quint16);
    explicit UdpSocket(const QHostAddress&, quint16, const QHostAddress&, quint16);
public:
    void sendPortData(const QByteArray&); //send to port
private Q_SLOTS:
    void receivePortData();
    void receiveTemperature();
public Q_SLOTS:
    void receiveData(const process_params_t&);
private:
    QUdpSocket* socket_;
    QHostAddress sender_addr_;
    QHostAddress receiver_addr_;
    quint16 sender_port_;
    quint16 receiver_port_;
private:
    QJsonObject json_;
signals:
    void sendData(const QString&); //send to core
    void sendTemperature(double); //send to core
};

}

#endif //UDP_H