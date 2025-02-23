#include "app.hpp"
#include "logger.hpp"
#include <QtConcurrent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>

namespace app {

QByteArray serializeVector(const std::vector<double>& vec) {
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    
    stream.setByteOrder(QDataStream::BigEndian);
    
    stream << static_cast<quint32>(vec.size());
    for (double val : vec) {
        stream << val;
    }
    return byteArray;
}

std::vector<double> deserializeResult(const QByteArray& data) {
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    quint32 size;
    stream >> size;

    std::vector<double> result;
    result.reserve(size);
    for (quint32 i = 0; i < size; ++i) {
        double val;
        stream >> val;
        result.push_back(val);
    }
    return result;
}

Application::Application(const QCoreApplication& q_core_app): q_core_app_(q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::EventType>();

    try {
        network_ = std::make_unique<Network>();

        udp_handler_ = std::make_unique<CommandHandler>();

        udp_handler_->setReceiverParameters(network_->getOwnIp(), 
                                       ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
        udp_handler_->setSenderParameters(QHostAddress(ConfigReader::getInstance().get("network", "hostIp").toString()), 
                                       ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());
        
#ifndef NOT_BLE
        runBle();
#endif
        runCore();

        auto camera_python_process_path = fs::current_path().parent_path() / 
            ConfigReader::getInstance().get("files", "camera_python_script").toString().toStdString();
        QStringList args = QStringList() << QString::fromStdString(camera_python_process_path.string());
        camera_python_.start ("python3", args);

        optimization_script_ = std::make_unique<QProcess>();

        connect(optimization_script_.get(), &QProcess::started, [this]() {
            std::vector data = {1.1,2.0,3.0,4.0};

            optimization_script_->write(serializeVector(data));
            optimization_script_->closeWriteChannel();
            
        });

        connect(optimization_script_.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                QByteArray resultData = optimization_script_->readAllStandardOutput();
                std::vector<double> result = deserializeResult(resultData);
                print(result);
                optimization_script_->terminate();
            } else {
                qDebug() << "Process failed:" << optimization_script_->errorString();
            }
        });

        connect(optimization_script_.get(), &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
            qDebug() << "Process error: " + QString::number(error);
        });
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

void Application::runCore() {

#ifndef NOT_BLE
    if(!ble_thread_.isRunning()) {
        throw std::runtime_error("The core can only be started after the startup BLE");
    }
#endif
    core_ = std::make_unique<Core>(std::make_shared<app::TestProcessUnit>());
    core_->moveToThread(&core_thread_);

    connect(&core_thread_, &QThread::started, 
        core_.get(), &app::Core::process, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::exit, 
        &core_thread_, &QThread::quit, Qt::QueuedConnection);

    connect(&core_thread_, &QThread::finished, 
        core_.get(), &QObject::deleteLater, Qt::QueuedConnection);

    connect(&core_thread_, &QThread::finished,
        &q_core_app_, &QCoreApplication::quit, Qt::QueuedConnection);

    connect(udp_handler_.get(), &app::CommandHandler::setCoreStatement, 
        core_.get(), &app::Core::setCoreStatement, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::sendData, 
        udp_handler_.get(), &app::UdpHandler::receiveData, Qt::QueuedConnection);
    connect(core_.get(), &app::Core::runOptimizationProcess, 
        this, &Application::runOptimizationProcess, Qt::QueuedConnection);

#ifndef NOT_BLE
    connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
        core_.get(), &app::Core::receiveTemperature, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::requestTemperature, 
        bluetoothDevice_.get(), &ble::BLEInterface::temperature, Qt::QueuedConnection);

    connect(bluetoothDevice_.get(), &ble::BLEInterface::isReady,
        core_.get(), &app::Core::setBlEStatus, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::setRateTemprature, 
        bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);
#endif
    core_thread_.start();
}

void Application::runBle() {
    bluetoothDevice_ = std::make_unique<ble::BLEInterface>();
    bluetoothDevice_->moveToThread(&ble_thread_);

    connect(&ble_thread_, &QThread::started, 
        bluetoothDevice_.get(), &ble::BLEInterface::run, Qt::QueuedConnection);

    connect(udp_handler_.get(), &app::CommandHandler::setRateTemprature, 
        bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);

    ble_thread_.start();
}

void Application::runOptimizationProcess() {
    optimization_script_->start ("python3", QStringList() << QString::fromStdString((fs::current_path().parent_path() / "optimization.py").string()));
}

Application::~Application()
{

}

} //namespace app