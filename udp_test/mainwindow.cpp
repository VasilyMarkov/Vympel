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
#include <math.h>
#include <map>
constexpr size_t frame_size = 10000;



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
template <typename T, typename U>
void print(const std::map<T, U>& map) {
    for(auto&& [key, value]:map) {
        std::cout << key << ": " << value << std::endl;
    }
}
double round_to(double value, double precision = 1.0)
{
    return std::round(value / precision) * precision;
}

constexpr size_t window_size = 100;
std::deque<double> wind(window_size, 0);
std::queue<double> socketData;

double g_mean = 0;

std::tuple<double, double> lineEquation(double x1, double y1, double x2, double y2) {
    double m = (y2 - y1) / (x2 - x1); // Slope
    double b = y1 - m * x1; // Y-intercept
    return std::make_tuple(m, b);
}

double findXIntercept(double m, double b) {
    if (m == 0) {
        throw std::invalid_argument("The slope cannot be zero");
    }
    return -b / m;
}

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
    plot->yAxis->setRange(-1, 1);
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


    var_pos_line = new QCPItemLine(plot);
    var_pos_line->setPen(QPen(QColor(240, 199, 50), 1.5, Qt::DotLine));
    var_neg_line = new QCPItemLine(plot);
    var_neg_line->setPen(QPen(QColor(240, 199, 50), 1.5, Qt::DotLine));

    start_line = new QCPItemLine(plot);
    start_line->setPen(QPen(QColor(65, 172, 242), 1.5, Qt::DotLine));
    end_line = new QCPItemLine(plot);
    end_line->setPen(QPen(QColor(65, 172, 242), 1.5, Qt::DotLine));
    max_line = new QCPItemLine(plot);
    max_line->setPen(QPen(QColor(65, 172, 242), 1.5, Qt::DotLine));

    intrcpt_line1 = new QCPItemLine(plot);
    intrcpt_line1->setPen(QPen(QColor(37, 8, 44), 2));
    intrcpt_line2 = new QCPItemLine(plot);
    intrcpt_line2->setPen(QPen(QColor(37, 8, 44), 2));
    hist = ui->hist;

    setupPlot(hist);

    calib_data.reserve(window_size);
    filtered_data.reserve(window_size);
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
    static bool detect_mode = false;
    static bool calib_done = false;

    static size_t x = 0;
    static size_t cnt = 0;
    std::vector<char> buffer;
    buffer.resize(socket->pendingDatagramSize());
    QByteArray datagram;
    datagram.resize(socket->pendingDatagramSize());
    socket->readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    QJsonParseError jsonErr;
    auto json = QJsonDocument::fromJson(datagram, &jsonErr);

    auto value = json.object().value("bright").toDouble();
    auto filtered = json.object().value("filtered").toDouble();
    auto low_passed = json.object().value("low_passed").toDouble();
    auto jmean = json.object().value("mean").toDouble();
    auto jvariance = json.object().value("variance").toDouble();

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
        fill_occurences();
        draw_hist(hist);
        cnt = 0;
        th = true;
        calib_done = true;
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
    auto new_filtered = static_cast<double>((filtered-g_mean))/max_value1;
    auto new_low_passed = static_cast<double>((low_passed-g_mean))/max_value1;

    plot->graph(0)->addData(x, new_value);
    plot->graph(1)->addData(x, new_filtered);
    auto norm_var = std::sqrt(jvariance)/max_value1;
    static size_t cnt_m = 0;
    if (calib_done) {
        filtered_data.push_back(new_filtered);
    }
    else {
        filtered_data.push_back(0);
    }
    updateQueue(new_filtered);
    switch(fsm) {
        static size_t detect_cnt = 0;
        static auto start_p = 0.0;
        static auto end_p = 0.0;
        case fsm_t::START:
            if (new_filtered > norm_var && calib_done) {
                std::cout << "START" << std::endl;
                start_p = x;
                fsm = fsm_t::DETECTION;
            }  
            if(x % 60 == 0) {
                static double mean_deque = 0;
                auto new_mean_deque = std::accumulate(std::begin(deque), std::end(deque), 0)/deque.size();
                if(deque.size() == 50) {
                    if (new_mean_deque > mean_deque) {

                    }
                    mean_deque = new_mean_deque;
                }

            }
        break;
        case fsm_t::DETECTION:

            if (new_filtered > norm_var) {
                ++cnt_m;
            }
            if (detect_cnt == 20 && (static_cast<double>(cnt_m)/detect_cnt) < 0.5) {
                detect_cnt = 0;
                fsm = fsm_t::START;
            }
            if (detect_cnt == 60 && (static_cast<double>(cnt_m)/detect_cnt) >= 0.9) {
//                std::cout << "DETECTION" << std::endl;
                detect_cnt = 0;

                fsm = fsm_t::MEASHURE;
            }
            ++detect_cnt;
        break;
        case fsm_t::MEASHURE:
//            std::cout << "MEASHURE" << std::endl;
            end_p = x;
            start_line->start->setCoords(start_p, -1);
            start_line->end->setCoords(start_p, 1);
            end_line->start->setCoords(end_p, -1);
            end_line->end->setCoords(end_p, 1);
            {
                auto [k, b] = lineEquation(start_p, filtered_data[start_p], end_p, filtered_data[end_p]);
                auto x_intercept = findXIntercept(k, b);
                intrcpt_line1->start->setCoords(x_intercept, 0);
                intrcpt_line1->end->setCoords(end_p, filtered_data[end_p]);
                std::cout << "START POINT: " << x_intercept << std::endl;
            }
            fsm = fsm_t::IDLE;
        break;
        case fsm_t::FULL:
//            std::cout << "FULL" << std::endl;
            std::cout << filtered_data.size() << std::endl;
            auto max_it = std::max_element(std::begin(filtered_data), std::end(filtered_data));
            auto max_pos = std::distance(std::begin(filtered_data), max_it);
//            std::cout << "max_pos: " << max_pos << ", max_val: " << *max_it << std::endl;
            std::map<size_t, double> elements;
            auto lower = *max_it*0.94;
            auto upper = *max_it*0.96;
            size_t index = 0;
            std::for_each(std::begin(filtered_data), std::end(filtered_data), [&](double elem)
            {
                if (elem > lower && elem < upper)
                    elements.emplace(index, elem);
                ++index;
            });
//            print(elements);
//            auto max_val_it = elements.upper_bound(max_pos);
            auto max_val_it = std::max_element(std::begin(elements), std::end(elements));
//            std::cout << "next: " << max_val_it->first << std::endl;
            max_line->start->setCoords(max_val_it->first, -1);
            max_line->end->setCoords(max_val_it->first, 1);
            std::cout << "END POINT: " << max_val_it->first << std::endl;
            fsm = fsm_t::END;
        break;
    }

    if (new_filtered > norm_var) {
        static size_t detect_cnt = 0;
        if(detect_mode) {
            if(detect_cnt == 20) {
                detect_cnt = 0;
            }
            ++detect_cnt;
        }
        detect_mode = true;
    }
    var_pos_line->start->setCoords(0, norm_var);
    var_pos_line->end->setCoords(frame_size, norm_var);
    var_neg_line->start->setCoords(0, -norm_var);
    var_neg_line->end->setCoords(frame_size, -norm_var);

    static size_t cnt1 = 0;
    static bool prc = false;
    if (new_value > 0.2 && th) {
        ui->begin->setText(QString("Start: %1").arg(cnt1));
        prc = true;
        th = false;
    }
    if (cnt1 > 3500 && new_value < 0.2 && prc) {
        ui->end->setText(QString("End: %1").arg(cnt1));
        prc = false;
    }
    cnt1++;
    if (auto data_cnt = plot->graph(0)->dataCount() == frame_size) {
        plot->xAxis->setRange(data_cnt, data_cnt+frame_size);
    }
    plot->replot();
    x++;
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
    wind.pop_front();
    plot->graph(0)->addData(x++, value);
    plot->replot();
}

void MainWindow::on_calibrate_clicked()
{
    calibration = true;
}

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

void MainWindow::updateQueue(double value)
{
    if(deque.size() == 50) {
        deque.pop_back();
    }
    deque.push_front(value);
}


void MainWindow::on_stop_clicked()
{
    if (fsm != fsm_t::FULL) {
        fsm = fsm_t::FULL;
    }
}

