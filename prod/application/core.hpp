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
    void setRateTemprature(float rate, float brightness, float cond_mark, float vapor_mark, float half_sum_mark);
    void runOptimizationProcess(const std::vector<double>&);
    void sendCompressedImage(const std::vector<uint8_t>&);
private:
    /**********FSM***********/
    void callEvent();
    void toggle(EventType);
    void dispatchEvent();
    void onFSM();
    void offFSM();
    void resetState();
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
    double temperatureRate_{};
    double setRate{};
    QTimer timer_;
    QTimer heat_timer_;
    int start_time_mark_{};
    int end_time_mark_{};
    bool called_once_ = false;
    size_t work_tick_{};
    float cond_temp_ = -200.;
    float vapor_temp_ = -200.;
    float half_sum_temp_ = -200.;
};



} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::EventType)

#endif //CORE_H