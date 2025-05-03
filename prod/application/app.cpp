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

        connect(&optimizationScript_, &OptimizationScript::sendCoefficients, network_.get(), &Network::receiveFuncCoefficients);

        connect(network_.get(), &Network::ready, [this](){

            udp_handler_->setReceiverParameters(network_->getOwnIp(), 
                ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
            udp_handler_->setSenderParameters(QHostAddress(network_->getHostIp().toIPv4Address()), 
                ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());

            runCore();
        });

        if(ConfigReader::getInstance().isBleEnable()) {
            runBle();
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

void Application::runCore() {

    auto module_type = ConfigReader::getInstance().get("parameters", "process_module").toString().toStdString();

    core_ = std::make_unique<Core>(moduleFactory_[module_type]());
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

    // connect(core_.get(), &app::Core::sendCompressedImage, 
    //     network_.get(), &Network::receiveCompressedImage, Qt::QueuedConnection);

    connect(&optimizationScript_, &OptimizationScript::sendCoefficients, core_.get(), &app::Core::receiveFitCoefficients);


    if(ConfigReader::getInstance().isBleEnable()) {
        connect(bluetoothDevice_.get(), &ble::BLEInterface::sendTemperature,
            core_.get(), &app::Core::receiveTemperature, Qt::QueuedConnection);

        connect(core_.get(), &app::Core::requestTemperature, 
            bluetoothDevice_.get(), &ble::BLEInterface::temperature, Qt::QueuedConnection);

        connect(bluetoothDevice_.get(), &ble::BLEInterface::isReady,
            core_.get(), &app::Core::setBlEStatus, Qt::QueuedConnection);

        connect(core_.get(), &app::Core::setRateTemprature, 
            bluetoothDevice_.get(), &ble::BLEInterface::changeRateTemprature, Qt::QueuedConnection);
    }

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
    if(ble_enable_) {
        ble_thread_.quit();
    }

    core_thread_.requestInterruption();
    core_thread_.quit();
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