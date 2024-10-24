#ifndef FSM_H
#define FSM_H

#include <memory>
#include "interface.hpp"
#include "event.hpp"

namespace app
{

class Fsm final {
public: 
    template<typename Event>
    using eventFactory = std::function<std::unique_ptr<Event>()>;
    
    template<typename Event>
    Fsm(eventFactory<Event>);
    
    void callEvent();
private:
    void toggle(core_mode_t);
    void dispatchEvent();
    core_mode_t mode_ = core_mode_t::IDLE;
    std::unique_ptr<Event> active_event_;
};

} //namespace app

#endif //FSM_H