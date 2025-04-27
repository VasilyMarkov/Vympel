#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <iostream>
#include <memory>
#include <list>
#include <unordered_map>
#include <functional>
#include <optional>
#include <deque>
#include "interface.hpp"
#include "event.hpp"
#include "fsm.hpp"


namespace app {

enum class CoreStatement {
    no_state,
    halt,
    work,
};

class Core final: public QObject, IReceiver, ISender {
    Q_OBJECT
    friend class CoreTest;
public:
    explicit Core(std::shared_ptr<IProcessing>);
    std::shared_ptr<IProcessing> getProcessUnit() const noexcept;
    
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
    void receiveTemperature(double) noexcept;
    bool process();
    void setBlEStatus() noexcept;
    void setCoreStatement(int) noexcept;
    void receiveRateTemprature(double) noexcept;
    void receiveFitCoefficients(const std::vector<double>&);
Q_SIGNALS:
    void sendData(const QJsonDocument&) const override;
    void exit();
    void requestTemperature();
    void setRateTemprature(double);
    void runOptimizationProcess(const std::vector<double>&);
    void sendCompressedImage(const std::vector<uint8_t>&);
private:
    /**********FSM***********/
    void callEvent();
    void toggle(EventType);
    void dispatchEvent();
    void onFSM();
    void offFSM();
    /************************/
    CoreStatement statement_ = CoreStatement::halt;
    EventType mode_ = EventType::NO_STATE;
    std::shared_ptr<IProcessing> process_unit_;
    std::unique_ptr<Event> active_event_;
    QJsonObject json_;
    std::vector<double> global_data_;
    std::vector<double> temperature_data_;
    double temperature_;
    bool bleIsReady_ = false;
    bool isOnFSM = false;
    double temperatureRate_;
    double setRate = 0.0;
    QTimer timer_;
    size_t start_mark_{};
    size_t end_mark_{};
    size_t work_tick_{};
};

inline double polyval(const std::vector<double>& coeffs, double x) noexcept {
    double tmp = 0.0;
    size_t degree = coeffs.size() - 1;
    for( auto&& coeff : coeffs) {
         tmp += coeff*std::pow(x, degree);
         --degree;
    }
    return tmp;
}

inline double gaussPolyVal(const std::vector<double>& coeffs, double x) noexcept {
    double a = coeffs[0];
    double mean = coeffs[1];
    double var = coeffs[2];
    double z = polyval(std::vector<double>(std::next(std::begin(coeffs), 3), std::end(coeffs)),(x-mean));
    return a*std::exp(-(0.5*z*z)/(var*var));
}

template<typename Func>
std::pair<std::vector<double>,std::vector<double>>  applyFunc(
    const std::vector<double>& coeffs,
    int begin,
    int end,
    size_t pointsNum,
    Func&& function)
{
    assert(end > begin);
    std::vector<double> x_data(pointsNum);
    double step = std::fabs(end-begin)/pointsNum;
    x_data[0] = begin;
    std::generate(std::next(std::begin(x_data)), std::end(x_data), [step, begin](){
        static double val = begin;
        return val += step;
    });

    std::vector<double> y_data;
    y_data.reserve(x_data.size());

    for(auto&& x : x_data) {
        y_data.push_back(function(coeffs, x));
    }
    return {x_data, y_data};
}

inline bool almostEqual(double lhs, double rhs, double eps) {
    return std::fabs(lhs-rhs) < eps;
}

} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::EventType)

#endif //CORE_H