#include "bleService.hpp"

BlService::BlService():timer_(new QTimer(this))  {
    timer_->setInterval(100);
    connect(timer_, &QTimer::timeout, this, &BlService::process);
}

void BlService::process() {
    emit temperature();
}

void BlService::run() {
    timer_->start();
}