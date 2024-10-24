#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <concepts>
#include <numeric>
#include <cmath>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QVariant>


namespace app {

namespace constants {
    namespace filter {
        constexpr double cutoff_frequency = 10.0; //Hz
        constexpr double cutoff_frequency1 = 50.0; //Hz
        constexpr double sample_rate = 1000.0; //Hz
    }
}

template <typename T>
void print(const std::vector<T>& vector) {
    for(auto&& el:vector) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

template <typename T, typename U>
void print(const std::unordered_map<T, U>& map) {
    for(auto&& el:map) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

inline std::vector<double> readInputData() {
    std::vector<double> data;
    double temp = 0;

    if(std::cin.fail()) throw std::runtime_error("Invalid input data");

    while (std::cin >> temp) {
        data.push_back(temp);
    }
    
    return data;
}

inline std::optional<QVariantMap> parseJsonFile(const QString &filePath) {

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << filePath;
        return std::nullopt;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return std::nullopt;
    }

    auto jsonObj = jsonDoc.object();

    auto networkObj = jsonObj["network"].toObject();
    auto cameraObj = jsonObj["camera"].toObject();

    auto clientIp = networkObj.value("clientIp").toString();
    auto clientPort = networkObj.value("clientPort").toInt();
    auto hostIp = networkObj.value("hostIp").toString();
    auto hostPort = networkObj.value("hostPort").toInt();

    QVariantMap config;

    config["clientIp"].setValue(clientIp);
    config["clientPort"].setValue(clientPort);
    config["hostIp"].setValue(hostIp);
    config["hostPort"].setValue(hostPort);

    return config;
}

inline uint16_t crc16(const std::vector<uint8_t>& buf, size_t len) {
    uint16_t nCRC16 = 0xFFFF;
    uint16_t tmp;

    if (!buf.empty() && len > 0 && len <= buf.size()) {
        for (int i = 0; i < len; i++) {
            tmp = static_cast<uint16_t>(0x00FF & buf[i]);
            nCRC16 ^= tmp;
            for (int k = 0; k < 8; k++) {
                tmp = static_cast<uint16_t>(nCRC16 & 0x0001);
                if (tmp == 0x0001) {
                    nCRC16 >>= 1;
                    nCRC16 ^= 0xA001;
                } else {
                    nCRC16 >>= 1;
                }
            }
        }
        return nCRC16;
    }
    return 0x0000;
}

/**
 * @brief meanVar
 * 
 * Calculates mean and variance
 * 
 * @param  std::vector<double> 
 * @return std::pair<double, double> 
 */
inline std::pair<double, double> meanVar(const std::vector<double>& data) {
    auto mean = std::accumulate(std::begin(data), std::end(data), 0.0)/data.size();
    auto sq_sum = std::inner_product(std::begin(data), std::end(data), std::begin(data), 0.0);
    return {mean, std::sqrt(sq_sum/data.size() - mean*mean)};
}

/**
 * @brief findLineCoeff
 * 
 * Function calculates gradient of line approximation of data (Least squares)
 * 
 * @param std::vector<double> 
 * @return double 
 */
inline double findLineCoeff(const std::vector<double>& y) 
{
    auto n = y.size();
    std::vector<double> x(n);
    std::iota(std::begin(x), std::end(x), 0.0);

    auto x_sum = std::accumulate(std::begin(x), std::end(x), 0.0);
    auto y_sum = std::accumulate(std::begin(y), std::end(y), 0.0);
    auto x2_sum = std::inner_product(std::begin(x), std::end(x), std::begin(x), 0.0);
    auto xy_sum = std::inner_product(std::begin(x), std::end(x), std::begin(y), 0.0);

    auto nominator = n*xy_sum-x_sum*y_sum;
    auto denominator = n*x2_sum-x_sum*x_sum;
    
    auto a = nominator/denominator;

    if (auto epsilon{1e-7}; std::fabs(denominator - epsilon) <= 0) throw std::runtime_error("Divide by zero in least squares");

    return a; 
}

/**
 * @brief Low Pass Filter
 * 
 * Cuts off all frequencies above the laser flicker frequency 120 Hz
 * 
 */
class LowPassFilter {
public:
    LowPassFilter(double cutoff_frequency, double sample_rate, double q = 0.707): 
        alpha_(std::sin(2 * M_PI * cutoff_frequency / sample_rate) / (2 * q)) {}

    double Process(double x) {
        y_ = alpha_ * (x - y_) + y_;
        return y_;
    }

private:
    double alpha_{};
    double y_{};
};

}//namespace app

#endif //UTILITY_H