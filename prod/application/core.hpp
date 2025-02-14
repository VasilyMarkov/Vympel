#ifndef CORE_H
#define CORE_H

#include <QObject>
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
    std::shared_ptr<IProcessing> getProcessUnit() const;
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
    void receiveTemperature(double) noexcept;
    bool process();
    void setBlEStatus();
    void setCoreStatement(int);
Q_SIGNALS:
    void sendData(const QJsonDocument&) const override;
    void exit();
    void requestTemperature();
    void setRateTemprature(double);
private:
    /**********FSM***********/
    void callEvent();
    void toggle(EventType);
    void dispatchEvent();
    void onFSM();
    void offFSM();
    void callOnce(CoreStatement);
    /************************/
    CoreStatement statement_ = CoreStatement::halt;
    EventType mode_ = EventType::IDLE;
    std::shared_ptr<IProcessing> process_unit_;
    std::unique_ptr<Event> active_event_;
    QJsonObject json_;
    std::vector<double> global_data_;
    std::vector<double> temperature_data_;
    double temperature_;
    bool bleIsReady_ = false;
    bool isOnFSM = false;
};

} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::EventType)

#endif //CORE_H