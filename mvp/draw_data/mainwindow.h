#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "qcustomplot.h"
#include <QThread>
#include <unordered_map>
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class core_mode_t {
    IDLE,
    CALIBRATION,
    MEASHUREMENT
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSocket();
    void mouseWheel();
    void on_calibrate_clicked();
    void on_meashurement_clicked();

    void on_idle_clicked();

private:
    void setupPlot(QCustomPlot*);
    void modeEval(core_mode_t);
    void sendData(const QByteArray&);
private:
    Ui::MainWindow *ui;
    QUdpSocket* socket;
    QCustomPlot* plot;
    std::unordered_map<core_mode_t, QString> modes_;
};
#endif // MAINWINDOW_H
