#ifndef APP_H
#define APP_H

#include <QUdpSocket>
#include <QCoreApplication>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QSocketNotifier>
#include <unordered_map>
#include <functional>
#include "core.hpp"
#include "network.hpp"
#include "cv.hpp"
#include "utility.hpp"
#include "bluetoothDevice.hpp"
#include "logger.hpp"
#include "configReader.hpp"

namespace app
{

class OptimizationScript final: public QObject {
    Q_OBJECT
public:
    OptimizationScript();
    ~OptimizationScript();
public Q_SLOTS:
    void start(const std::vector<double>&);
Q_SIGNALS:
    void sendCoefficients(const std::vector<double>&);
private:    
    QByteArray serializeVector(const std::vector<double>&);
    std::vector<double> deserializeResult(const QByteArray&);
private:
    std::unique_ptr<QProcess> process_;
    QString processPath_;
    std::vector<double> data_;
    std::vector<double> coefficents_;
};

class Application final: public QObject {
    Q_OBJECT
public:
    Application(const QCoreApplication&);
    ~Application();
private:
    void runCore();
    void runBle();
private:
    std::unique_ptr<CommandHandler> udp_handler_;
    std::unique_ptr<UdpHandler> ble_socket_;
    std::unique_ptr<Network> network_;
    std::unique_ptr<Core> core_;
    std::unique_ptr<ble::BLEInterface> bluetoothDevice_;
    QThread core_thread_;
    QThread ble_thread_;
    QProcess camera_python_;
    OptimizationScript optimizationScript_;
    const QCoreApplication& q_core_app_;
    std::unordered_map<std::string, std::function<std::shared_ptr<IProcessing>()>> moduleFactory_ = 
        {
            {"camera", [](){return std::make_shared<app::CameraProcessingModule>();}},
            {"test", [](){return std::make_shared<app::TestProcessModule>();}}
        };
    bool ble_enable_ = true;
};


} // namespace app


#endif //APP_H