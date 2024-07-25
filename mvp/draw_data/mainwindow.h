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

Q_DECLARE_METATYPE(core_mode_t)

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

    void on_start_clicked();

    void on_stop_clicked();

    void on_disconnect_clicked();

private:
    void setupPlot(QCustomPlot*);
    void modeEval(core_mode_t);
    void sendData(const QByteArray&);
    QJsonValue toJson(core_mode_t) const;
private:
    Ui::MainWindow *ui;
    QUdpSocket* socket;
    QCustomPlot* plot;
};
#endif // MAINWINDOW_H
