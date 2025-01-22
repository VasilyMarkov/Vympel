#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "qcustomplot.h"
#include <QThread>
#include <unordered_map>
#include <deque>
#include "udp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class EventType {
    IDLE,
    CALIBRATION,
    MEASHUREMENT,
    CONDENSATION,
    END
};

enum class CoreStatement {
    WORK,
    HALT
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void receiveData(const QJsonDocument& json);
    void mouseWheel();

    void on_runCV_clicked();
    void on_stopCV_clicked();
    void on_startCV_clicked();
    void on_closeCV_clicked();

    void on_ble_connect_clicked();

Q_SIGNALS:
    void sendData(const QJsonDocument&);

private:
    void setupPlot(QCustomPlot*);
    void modeEval(EventType);
private:
    Ui::MainWindow *ui;
    QProcess process_;
    QStringList pythonCommandArguments_;
    std::unique_ptr<UdpSocket> socket_;
    QCustomPlot* plot;
    std::unordered_map<EventType, QString> modes_;
    CoreStatement coreStatement_;
    int sample_ = 0;
};
#endif // MAINWINDOW_H
