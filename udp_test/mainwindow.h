#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "qcustomplot.h"
#include <QThread>
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class fsm_t {
    START,
    IDLE,
    DETECTION,
    MEASHURE,
    FULL,
    END
};

class PythonScript: public QObject {
    Q_OBJECT

public:
    QProcess python;
    void run() {
        python.start("python3", QStringList() << "/home/vasily/usr/phystech/vympel/finish/send_data.py");
        python.waitForFinished();
    }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void readSocket();
    void plotData();
    void on_calibrate_clicked();

    void on_start_clicked();
    void script_output(std::string& str);
    void on_stop_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket* socket;
    QCustomPlot* plot;
    QCustomPlot* hist;
    QCPItemLine* line;
    QCPItemLine* start_line;
    QCPItemLine* end_line;
    QCPItemLine* var_pos_line;
    QCPItemLine* var_neg_line;
    PythonScript* script;
    QThread* python_thread;
    std::vector<QCPItemLine*> lines;
    std::vector<double> calib_data;
    std::unordered_map<double, size_t> occurences;
    std::vector<double> filtered_data;
    fsm_t fsm = fsm_t::START;
    double g_mean = 1;
    double g_max = 1;
    double g_sigma = 0;
    bool calibration = false;
    void setupPlot(QCustomPlot*);
    void fill_occurences();
    void draw_hist(QCustomPlot*);
};
#endif // MAINWINDOW_H
