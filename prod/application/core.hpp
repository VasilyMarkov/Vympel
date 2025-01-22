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
    WORK,
    HALT
};

class Core final: public QObject, IReceiver, ISender {
    Q_OBJECT
    friend class CoreTest;
public:
    explicit Core(std::shared_ptr<IProcessing>);
    std::shared_ptr<IProcessing> getProcessUnit() const;
public Q_SLOTS:
    void receiveData(const QJsonDocument&) override;
    void receiveTemperature(double) const;
    bool process();
    void bleDeviceConnected();
Q_SIGNALS:
    void sendData(const QJsonDocument&) const override;
    void exit();
    void requestTemperature();
private:
    /**********FSM***********/
    void callEvent();
    void toggle(EventType);
    void dispatchEvent();
    void onFSM();
    void offFSM();
    bool isOnFSM = false;
    /************************/
    CoreStatement statement_ = CoreStatement::HALT;
    EventType mode_ = EventType::IDLE;
    std::shared_ptr<IProcessing> process_unit_;
    std::unique_ptr<Event> active_event_;
    QJsonObject json_;
    std::vector<double> global_data_;
};

} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::EventType)

#endif //CORE_H