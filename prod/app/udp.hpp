#ifndef UDP_H
#define UDP_H
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QJsonObject>
#include <qt6/QtCore/QJsonDocument>
#include "interface.hpp"

namespace app {

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
    void receiveData(const cv_params_t&);
private:
    QUdpSocket socket_;
    QHostAddress sender_addr_ = QHostAddress::LocalHost;
    QHostAddress receiver_addr_ = QHostAddress::LocalHost;
    quint16 sender_port_ = 1024;
    quint16 receiver_port_ = 1024;
private:
    QJsonObject json_;
signals:
    void sendData(const QString&); //send to core
};

}

#endif //UDP_H