#ifndef CONFIG_READER_H
#define CONFIG_READER_H
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
#include <exception>
#include <cstdarg>
#include <QHash>

namespace app 
{

namespace fs = std::filesystem;

class ConfigReader final 
{
public:
    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    static ConfigReader& getInstance()
    {
        static ConfigReader instance;
        return instance;
    }

    QVariant get(QString globalKey, QString key) const {
        return (*config_store_[globalKey])[key];
    }
private:
    ConfigReader() {
        auto configFilePath = fs::current_path().parent_path();
        configFilePath /= localPath_;
        
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Could not open config file.");
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            throw std::runtime_error("JSON parse error.");
        }

        auto hash = jsonDoc.toVariant().toHash();

        for(auto it = hash.begin(); it != hash.end(); ++it) {
            config_store_.emplace(it.key(), std::make_shared<QVariantHash>(it.value().toHash()));
        }
    }
    ~ConfigReader(){}

    std::string localPath_ = "conf/config.json";
    QHash<QString, std::shared_ptr<QVariantHash>> config_store_;
};

inline auto&& configReader = ConfigReader::getInstance();

} //namespace app

#endif //CONFIG_READER_H