#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QJsonObject>
#include <algorithm>
#include <deque>
#include <queue>
#include <iostream>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QVariant>
#include <math.h>
#include <map>
#include "configreader.h"

constexpr size_t frame_size = 2000;
constexpr double VALUE_SIZE = 3e6;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , modes_({
        {core_mode_t::IDLE, "idle"},
        {core_mode_t::CALIBRATION, "calibration"},
        {core_mode_t::MEASHUREMENT, "meashurement"}
    })
{
    ui->setupUi(this);
    socket_ = std::make_unique<UdpSocket>();
    socket_->setReceiverParameters(QHostAddress(configReader.get("network", "clientIp").toString()),
                                   configReader.get("network", "serviceProgramPort").toInt());
    socket_->setSenderParameters(QHostAddress(configReader.get("network", "clientIp").toString()),
                                   configReader.get("network", "controlFromServiceProgramPort").toInt());

    connect(socket_.get(), &UdpSocket::sendData, this, &MainWindow::receiveData);

    resize(1280, 720);
    plot = ui->plot;
    connect(plot, &QCustomPlot::mouseWheel, this, &MainWindow::mouseWheel);
    setupPlot(plot);
    plot->yAxis->setRange(-VALUE_SIZE, 5*VALUE_SIZE);
    plot->xAxis->setRange(0, frame_size);

    plot->addGraph();
    plot->addGraph();

    plot->graph(0)->setPen(QPen(QColor(82, 247, 79), 2));
    plot->graph(1)->setPen(QPen(QColor(242, 65, 65), 2));
    plot->graph(0)->setName("brightness");
    plot->graph(1)->setName("filtered");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveData(const QJsonDocument& json)
{
    static int sample = 0;
    auto brightness = json["brightness"].toDouble();
    auto filtered = json["filtered"].toDouble();

    plot->graph(0)->addData(sample, brightness);
    plot->graph(1)->addData(sample, filtered);
    sample++;
    plot->replot();
}

void MainWindow::mouseWheel()
{
    if (plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
      plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
    else if (plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
      plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
    else
      plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::setupPlot(QCustomPlot* plot)
{
    plot->xAxis->setBasePen(QPen(Qt::white, 1));
    plot->yAxis->setBasePen(QPen(Qt::white, 1));
    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(true);
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    plot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    plot->axisRect()->setBackground(axisRectGradient);
    plot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(100);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    plot->legend->setFont(legendFont);

}

void MainWindow::modeEval(core_mode_t mode)
{
    switch (mode) {
        case core_mode_t::IDLE:
            ui->status->setText("IDLE");
        break;
        case core_mode_t::CALIBRATION:
            ui->status->setText("CALIBRATION");
        break;
        case core_mode_t::MEASHUREMENT:
            ui->status->setText("MEASHUREMENT");
        break;
        default:
            ui->status->setText("IDLE");
        break;
    }
}

void MainWindow::sendData(const QByteArray& data)
{
    if(data.isEmpty()) return;
//    socket->writeDatagram(data, QHostAddress::LocalHost, 65001);
}

void MainWindow::on_calibrate_clicked()
{
    QJsonObject json;
    json["core_mode"] = modes_.at(core_mode_t::CALIBRATION);
    sendData(QJsonDocument(json).toJson());
}

void MainWindow::on_meashurement_clicked()
{
    QJsonObject json;
    json["core_mode"] = modes_.at(core_mode_t::MEASHUREMENT);
    sendData(QJsonDocument(json).toJson());
}


void MainWindow::on_idle_clicked()
{
    QJsonObject json;
    json["core_mode"] = modes_.at(core_mode_t::IDLE);
    sendData(QJsonDocument(json).toJson());
}

