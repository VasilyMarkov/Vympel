#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <algorithm>
#include <deque>
#include <queue>
#include <iostream>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <math.h>

constexpr size_t frame_size = 1000;

template <typename Cont>
void print(const Cont& cont) {
//    std::copy(std::begin(cont), std::end(cont), std::ostream_iterator<T>(std::cout, ", "));
//    std::cout << std::endl;
    for(auto&& elem:cont) {
        std::cout << elem << ' ';
    }
    std::cout << std::endl;
}
template <typename T, typename U>
void print(const std::unordered_map<T, U>& map) {
    for(auto&& [key, value]:map) {
        std::cout << key << ": " << value << std::endl;
    }
}

double round_to(double value, double precision = 1.0)
{
    return std::round(value / precision) * precision;
}

constexpr const size_t window_size = 200;
std::deque<double> wind(window_size, 0);
std::queue<double> socketData;

double g_mean = 0;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    socket = new QUdpSocket(nullptr);
    ui->setupUi(this);
    socket->bind(QHostAddress("0.0.0.0"), 65000);
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::readSocket);
    plot = ui->plot;
    setupPlot(plot);
    plot->yAxis->setRange(-0.5, 0.5);
    plot->xAxis->setRange(0, frame_size);

    plot->addGraph();
    plot->addGraph();
    plot->addGraph();
    plot->addGraph();
    plot->graph(0)->setPen(QPen(QColor(82, 247, 79), 2));
    plot->graph(1)->setPen(QPen(QColor(242, 65, 65), 2));
    plot->graph(2)->setPen(QPen(QColor(65, 172, 242), 2));
    plot->graph(3)->setPen(QPen(QColor(242, 204, 65), 2));

    line = new QCPItemLine(plot);
    line->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));
    lines.push_back(new QCPItemLine(plot));
    lines.back()->setPen(QPen(QColor(250, 250, 62), 0.5));


    hist = ui->hist;

    setupPlot(hist);

//    QCPBars* bar = new QCPBars(hist->xAxis, hist->yAxis);
//    bar->setAntialiased(false);
//    bar->setName("Regenerative");
//    bar->setPen(QPen(QColor(0, 168, 140).lighter(130)));
//    bar->setBrush(QColor(0, 168, 140));
//    bar->setWidth(0.01);
//    QVector<double> data = {1, 2};
//    QVector<double> ticks = {1, 2};
//    bar->setData(data, ticks);

//    QTimer* threadTimer = new QTimer();
//    connect(threadTimer, &QTimer::timeout, this, [&](){
//            QtConcurrent::run(this, &MainWindow::plotData);}, Qt::QueuedConnection);
//    threadTimer->start(20);
//    connect(&script, SIGNAL(readyReadStandardOutput()), this, );
    calib_data.reserve(window_size);
    python_thread = new QThread();
    script = new PythonScript();
//    script->run();
    script->moveToThread(python_thread);
    connect(python_thread, &QThread::started, script, &PythonScript::run);
}

MainWindow::~MainWindow()
{
    delete ui;
}

double mean(std::deque<double>& window) {
    return std::accumulate(std::begin(window), std::end(window), 0)/window.size();
}

std::pair<double, double> mean_var(const std::vector<double>& window) {
    auto mean = std::accumulate(std::begin(window), std::end(window), 0.0)/window.size();
    auto sq_sum = std::inner_product(std::begin(window), std::end(window), std::begin(window), 0.0);
    return {mean, std::sqrt(sq_sum/window.size() - mean*mean)};
}

void MainWindow::readSocket()
{
    static size_t x = 0;
    static size_t cnt = 0;
    std::vector<char> buffer;
    buffer.resize(socket->pendingDatagramSize());
    socket->readDatagram(buffer.data(), buffer.size(), nullptr, nullptr);
    std::vector<char> tmp(buffer.rbegin(), buffer.rend());
    char dat[4] = {buffer[0], buffer[1], buffer[2], buffer[3]};
    int value = 0;
    memcpy(&value, dat, 4);
    static bool th = false;
    if (cnt == window_size) {
        calibration = false;
        auto [mean, var] = mean_var(calib_data);
        g_mean = mean;
        g_sigma = std::sqrt(var);
        std::transform(calib_data.begin(), calib_data.end(), calib_data.begin(),
                  std::bind2nd(std::minus<double>(), mean));
        auto max = std::max_element(std::begin(calib_data), std::end(calib_data));
        g_max = *max;
        std::transform(calib_data.begin(), calib_data.end(), calib_data.begin(),
                  std::bind2nd(std::divides<double>(), 2*M_PI*g_sigma));
        for(auto&& elem : calib_data) {
            elem = round_to(elem, 0.01);
        }
        auto [mean1, var1] = mean_var(calib_data);

        ui->mean_label->setText(QString::number(mean1));
        ui->std_label->setText(QString::number(std::sqrt(var1)));
        ui->status->setText("Calibration is done");
//        print(calib_data);
        std::cout << "Mean: " << mean1 << ", std: " << std::sqrt(var1) << std::endl;
        fill_occurences();
        draw_hist(hist);
//        print(occurences);
        cnt = 0;
        th = true;
    }
    if (calibration) {
        ui->status->setText(QString("Calibration... %1").arg(cnt));
        calib_data.push_back(value);
        cnt++;
    }
    auto max_value = 0.0;
    auto min_value = 0.0;
    if (value > max_value) max_value = value;
    if (value < min_value) min_value = value;

    auto max_value1 = 0.01;
    if (value > max_value1) max_value1 = value;
    auto new_value = static_cast<double>((value-g_mean))/max_value1;

    plot->graph(0)->addData(x++, new_value);
//    std::cout << new_value << std::endl;

    static size_t cnt1 = 0;
    static bool prc = false;
    if (new_value > 0.2 && th) {
//        std::cout << "Start" << cnt1 << std::endl;
        ui->begin->setText(QString("Start: %1").arg(cnt1));
        prc = true;
        th = false;
    }
    if (cnt1 > 3500 && new_value < 0.2 && prc) {
//        std::cout << "End" << cnt1 << std::endl;
        ui->end->setText(QString("End: %1").arg(cnt1));
        prc = false;
    }
    cnt1++;
    plot->yAxis->setRange(-0.5, 0.5);
    if (auto data_cnt = plot->graph(0)->dataCount() == frame_size) {
        plot->xAxis->setRange(data_cnt, data_cnt+frame_size);
    }
    plot->replot();
//    std::cout << value << std::endl;
}


void MainWindow::plotData() {
    static size_t x = 0;
    std::vector<char> buffer;
    buffer.resize(socket->pendingDatagramSize());
    socket->readDatagram(buffer.data(), buffer.size(), nullptr, nullptr);
    std::vector<char> tmp(buffer.rbegin(), buffer.rend());
    char dat[4] = {buffer[0], buffer[1], buffer[2], buffer[3]};
    int value = 0;
    memcpy(&value, dat, 4);
    wind.push_back(value);
    auto value_mean = mean(wind);
    auto max = std::max_element(std::begin(wind), std::end(wind));
    std::cout << value << ' ' << value_mean << std::endl;
    wind.pop_front();
    plot->graph(0)->addData(x++, value);
    plot->replot();
}

void MainWindow::on_calibrate_clicked()
{
    calibration = true;
}

//void MainWindow::plotData()
//{
//    static size_t x = 0;
//    auto value = 0;
//    static size_t cnt_max = 0;
//    if(socketData.empty()) return;

//    value = socketData.front();
//    socketData.pop();
//    auto sign = 0;
//    auto threshold = 3.5;
//    auto influence = 0.2;
//    auto [mean, stdev] = filter(wind);
//    static auto max = 0;
//    static auto cnt = 0;
//    static auto flag = false;
//    if(std::fabs(value-mean) > threshold*stdev && stdev !=0) {
//        value > stdev ? sign = 100 : sign *= -100;
//        wind.push_back(value*influence +(1-influence)*wind.front());
//        if(!flag) {

//        }
//        if (value > max && value > 100) {
//            max = value;
//            lines[cnt]->start->setCoords(x, -200);
//            std::cout << x << std::endl;
//            lines[cnt]->end->setCoords(x, -200);
//        }
//        flag = true;

//    }
//    else {
//        sign = 0;
//        wind.push_back(value);
//        max = 0;
//        cnt++;
//        flag = false;
//    }
//    wind.pop_front();
//    plot->graph(0)->addData(x++, value);
//    plot->graph(1)->addData(x++, mean);
//    plot->graph(2)->addData(x++, stdev);
////    plot->graph(3)->addData(x++, sign);
//    plot->replot();
//}





void MainWindow::on_start_clicked()
{
    if (python_thread == nullptr)
        return;
    python_thread->start();
}

void MainWindow::script_output(std::string& str)
{

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
}

void MainWindow::fill_occurences()
{
    for(auto&& elem:calib_data){

        auto cnt = std::count(std::begin(calib_data), std::end(calib_data), elem);
        occurences.emplace(elem, cnt);
    }
}

void MainWindow::draw_hist(QCustomPlot* hist)
{
    QCPBars* bar = new QCPBars(hist->xAxis, hist->yAxis);
    bar->setAntialiased(false);
    bar->setName("Regenerative");
    bar->setPen(QPen(QColor(0, 168, 140).lighter(130)));
    bar->setBrush(QColor(0, 168, 140));
    bar->setWidth(1/window_size);

    auto min = std::min_element(std::begin(occurences), std::end(occurences));
    auto max = std::max_element(std::begin(occurences), std::end(occurences));
    std::cout << max->second << std::endl;


    QVector<double> data;
    data.reserve(window_size);
    QVector<double> ticks;
    ticks.reserve(window_size);
    for(auto&& [key, value]: occurences) {
        data.push_back(key);
        ticks.push_back(value);
    }
    auto max_ticks = std::max_element(std::begin(ticks), std::end(ticks));
    hist->xAxis->setRange(min->first, max->first);
    hist->yAxis->setRange(0, *max_ticks);
    bar->setData(data, ticks);
}

