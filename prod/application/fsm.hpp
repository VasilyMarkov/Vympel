#ifndef FSM_H
#define FSM_H

#include <memory>
#include <deque>
#include "interface.hpp"
#include "event.hpp"

namespace app
{

class Fsm final {
public:
    Fsm(std::weak_ptr<IProcessing>, const std::function<void()>&);
    void toggle(core_mode_t);
    void callEvent();
    void dispatchEvent();
private:
    core_mode_t mode_ = core_mode_t::IDLE;
    std::weak_ptr<IProcessing> process_unit_;
    std::unique_ptr<Event> active_event_;
    std::function<void()> request_temperature_callback_;
};

} //namespace app

#endif //FSM_H