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
#include <filesystem>
#include <fstream>  
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>

namespace app 
{

namespace fs = std::filesystem;

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
        // if(jsonDir.cdUp()) 
        // {
        //     if(QDir dir{QString(jsonDir.absolutePath() + "/logs")}; !dir.exists()) {
        //         jsonDir.mkdir("logs");
        //     }

        //     jsonDir.cd(QString(jsonDir.absolutePath() + "/logs"));
        //     auto fileName = QDateTime::currentDateTime().toString(Qt::ISODate);
        //     file = std::make_unique<QFile>(QString(jsonDir.absolutePath() + '/' + fileName + ".json"));        
        // }

        auto logsDirPath = fs::current_path().parent_path() / "logs";

        if(!fs::exists(logsDirPath)) {
            fs::create_directory(logsDirPath);
        }

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *now_tm = std::localtime(&now_time);

        std::stringstream sstream;
        sstream << std::put_time(now_tm, "%Y-%m-%d_%H:%M:%S.json");
        auto logPath = logsDirPath / sstream.view();
        logfile_ = std::make_unique<std::ofstream>(logPath);
    }
    void closeLog() {
        if (file->isOpen()) file->close();
    }
    void log(double data) 
    {
        if(!logfile_->is_open()) {
            throw std::runtime_error("Log file doesn't exist");
        }

        QJsonObject jsonObj;
        jsonObj["filtered_brightness"] = data;

        QJsonDocument jsonDoc(jsonObj);

        QByteArray jsonByteArray = jsonDoc.toJson();

        logfile_->write(jsonByteArray.data(), jsonByteArray.size());
    }

    void log(const std::vector<double>& data, const std::vector<double>& temperature) 
    {
        if(!logfile_->is_open()) {
            throw std::runtime_error("Log file doesn't exist");
        }

        if(data.empty() || temperature.empty()) {
            throw std::runtime_error("Error log");
        }

        std::vector<std::pair<double, double>> mergedData;
        mergedData.reserve(data.size());

        std::transform(std::begin(data), std::end(data), std::begin(temperature), 
            std::back_inserter(mergedData),
            [](double a, double b){return std::make_pair(a,b);});

        QJsonArray jsonArray;

        for (auto&& pair : mergedData) {
            QJsonObject jsonObject;
            jsonObject["brightness"] = pair.first;
            jsonObject["temperature"] = pair.second;
            jsonArray.append(jsonObject);
        }
        QJsonDocument jsonDoc(jsonArray);

        QByteArray jsonByteArray = jsonDoc.toJson();

        logfile_->write(jsonByteArray.data(), jsonByteArray.size());

        // closeLog();
    }
private:
    Logger():jsonDir(QCoreApplication::applicationDirPath()){}
    ~Logger(){ if (file->isOpen()) file->close(); }
    Logger(const Logger&){}
    Logger& operator=(const Logger&);

    QDir jsonDir;
    std::unique_ptr<QFile> file;
    std::unique_ptr<std::ofstream> logfile_;
};

} //namespace app

#endif //LOGGER_H