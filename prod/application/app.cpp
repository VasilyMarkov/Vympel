#include "app.hpp"
#include "logger.hpp"
#include <QtConcurrent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>

namespace app {

Application::Application(const QCoreApplication& q_core_app): q_core_app_(q_core_app)
{
    qRegisterMetaType<app::process_params_t>();
    qRegisterMetaType<app::EventType>();

    try {
        network_ = std::make_unique<Network>();

        udp_handler_ = std::make_unique<CommandHandler>();

        connect(&camera_python_, &QProcess::readyReadStandardOutput, [this](){qDebug() << camera_python_.readAllStandardOutput();});
        connect(&optimizationScript_, &OptimizationScript::sendCoefficients, network_.get(), &Network::receiveFuncCoefficients);
        // connect(&camera_python_, &QProcess::readyReadStandardError, [this](){qDebug() << camera_python_.readAllStandardError();});

        connect(network_.get(), &Network::ready, [this](){

            udp_handler_->setReceiverParameters(network_->getOwnIp(), 
                ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
            udp_handler_->setSenderParameters(QHostAddress(network_->getHostIp().toIPv4Address()), 
                ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());

            auto camera_python_process_path = fs::current_path().parent_path() / 
                ConfigReader::getInstance().get("files", "camera_python_script").toString().toStdString();
            QStringList args;
            args << QString::fromStdString(camera_python_process_path.string());
            args << network_->getHostIp().toString();
            // camera_python_.start ("python3", args);
            
            runCore();
        });

#ifndef NOT_BLE
        runBle();
#endif
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

    connect(&core_thread_, &QThread::finished, core_.get(),  &QObject::deleteLater);

    connect(udp_handler_.get(), &app::CommandHandler::setCoreStatement, 
        core_.get(), &app::Core::setCoreStatement, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::sendData, 
        udp_handler_.get(), &app::CommandHandler::receiveData, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::runOptimizationProcess, 
        &optimizationScript_, &OptimizationScript::start, Qt::QueuedConnection);

    connect(udp_handler_.get(), &app::CommandHandler::closeApp, 
        &q_core_app_, &QCoreApplication::quit, Qt::QueuedConnection);

    connect(core_.get(), &app::Core::sendCompressedImage, 
        network_.get(), &Network::receiveCompressedImage, Qt::QueuedConnection);


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

    connect(&ble_thread_, &QThread::finished, bluetoothDevice_.get(),  &QObject::deleteLater);

    connect(udp_handler_.get(), &app::CommandHandler::setRateTemprature, 
        bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);

    ble_thread_.start();
}

Application::~Application()
{
    #ifndef NOT_BLE
        ble_thread_.quit();
    #endif

    core_thread_.requestInterruption();
    core_thread_.quit();


    // camera_python_.terminate();
    // if (!camera_python_.waitForFinished(3000)) { // Wait for 5 seconds
    //     qDebug() << "Process did not terminate gracefully, killing it.";
    //     camera_python_.kill();
    // }

    camera_python_.kill();
    // wait for the process to actually stop
    camera_python_.waitForFinished();
}

OptimizationScript::OptimizationScript(): 
    process_(std::make_unique<QProcess>()), 
    processPath_(QString::fromStdString((fs::current_path().parent_path() / "optimization.py").string()))
{
    connect(process_.get(), &QProcess::readyReadStandardError, [this](){qDebug() << process_->readAllStandardError();});

    connect(process_.get(), &QProcess::started, [this]() {
        process_->write(serializeVector(data_));
        process_->closeWriteChannel();
        
    });

    connect(process_.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QByteArray resultData = process_->readAllStandardOutput();
            coefficents_ = deserializeResult(resultData);
            print(coefficents_);
            Q_EMIT sendCoefficients(coefficents_);
            process_->terminate();
        } else {
            qDebug() << "Process failed:" << process_->errorString();
        }
    });
}

void OptimizationScript::start(const std::vector<double>& data) {
    data_ = data;
    process_->start ("python3", QStringList() << processPath_);
}

QByteArray OptimizationScript::serializeVector(const std::vector<double>& vec) {
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    
    stream.setByteOrder(QDataStream::BigEndian);
    
    stream << static_cast<quint32>(vec.size());
    for (double val : vec) {
        stream << val;
    }
    return byteArray;
}

std::vector<double> OptimizationScript::deserializeResult(const QByteArray& data) {
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

OptimizationScript::~OptimizationScript() {
    if (process_->state() == QProcess::Running) {
        process_->kill();
    }


}

} //namespace app