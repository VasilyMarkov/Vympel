#include "bleService.hpp"

BlService::BlService():timer_(new QTimer(this)), bluetoothDevice_(std::make_unique<app::ble::BLEInterface>())  {
    timer_->setInterval(500);
    connect(timer_, &QTimer::timeout, this, &BlService::process);

    auto [clientIp, clientPort] = app::parseJsonFile(
        "/home/vasily/usr/phystech/vympel/prod/conf/config.json").value();

    socket_ = std::make_unique<app::network::UdpSocket>(QHostAddress(clientIp), clientPort, 
        QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    connect(bluetoothDevice_.get(), &app::ble::BLEInterface::deviceConnected, 
        this, &BlService::run, Qt::QueuedConnection);
    connect(bluetoothDevice_.get(), &app::ble::BLEInterface::sendTemperature, 
        socket_.get(), &app::network::UdpSocket::receiveData, Qt::QueuedConnection);
}

void BlService::process() {
    bluetoothDevice_->temperature();
}

void BlService::run() {
    timer_->start();
}