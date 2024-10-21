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

class Core final: public QObject, public ICommunication {
    Q_OBJECT
    friend class CoreTest;
public:
    explicit Core(std::shared_ptr<IProcessing>);
    std::shared_ptr<IProcessing> getProcessUnit() const;
public Q_SLOTS:
    void receiveData(const QString&) override;
    void receiveTemperature(double) const;
    bool process();
    void bleDeviceConnected();
Q_SIGNALS:
    void sendData(const process_params_t&) const override;
    void exit();
    void requestTemperature();
private:
    std::shared_ptr<IProcessing> process_unit_;
    std::unique_ptr<Fsm> fsm_;
    const std::unordered_map<QString, core_mode_t> events_;
};

} //namespace app

Q_DECLARE_METATYPE(app::process_params_t)
Q_DECLARE_METATYPE(app::core_mode_t)

#endif //CORE_H