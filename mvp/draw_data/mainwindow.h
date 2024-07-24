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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void readSocket();
    void mouseWheel();
private:
    void setupPlot(QCustomPlot*);
private:
    Ui::MainWindow *ui;
    QUdpSocket* socket;
    QCustomPlot* plot;
};
#endif // MAINWINDOW_H
