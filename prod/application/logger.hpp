#ifndef LOGGER_H
#define LOGGER_H
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <iostream>

namespace app 
{

class Logger final 
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }
    void createLog() 
    {
        if(jsonDir.cdUp()) 
        {
            if(QDir dir{QString(jsonDir.absolutePath() + "/logs")}; !dir.exists()) {
                jsonDir.mkdir("logs");
            }

            jsonDir.cd(QString(jsonDir.absolutePath() + "/logs"));
            std::cout << jsonDir.absolutePath().toStdString() << std::endl;
            auto fileName = QDateTime::currentDateTime().toString(Qt::ISODate);
            file = std::make_unique<QFile>(QString(jsonDir.absolutePath() + '/' + fileName + ".json"));
            // if (!file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            //     qWarning() << "Could not open file for writing:";
            //     return;
            // }

        }
    }
    void log(double data) 
    {
        QJsonObject jsonObj;
        jsonObj["filtered_brightness"] = data;

        QJsonDocument jsonDoc(jsonObj);
        QTextStream out(file.get());
        out << jsonDoc.toJson() << "\n";
    }

    void log(const std::vector<double>& data) 
    {
        QJsonArray jsonArray;

        std::copy(std::begin(data), std::end(data), std::back_inserter(jsonArray));
        QJsonDocument jsonDoc(jsonArray);

        if (file->open(QIODevice::WriteOnly)) {
            QTextStream stream(file.get());
            stream << jsonDoc.toJson();
            file->close();
        } 
        else {
            std::cerr << "Failed to open file for writing: " << std::endl;
        }

    }

    void destroyLog() 
    {
        if (file->isOpen()) file->close();
    }
private:
    Logger():jsonDir(QCoreApplication::applicationDirPath())
    {
        // QDir dir(QCoreApplication::applicationDirPath());
        // jsonDirectory = dir.absoluteFilePath(QCoreApplication::applicationName());


    }
    ~Logger(){}
    Logger(const Logger&){}
    Logger& operator=(const Logger&);

    // QString jsonDirectory;
    QDir jsonDir;
    std::unique_ptr<QFile> file;
};

inline auto&& logger = Logger::getInstance();

} //namespace app

#endif //LOGGER_H