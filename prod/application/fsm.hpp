#ifndef FSM_H
#define FSM_H

#include <memory>
#include "interface.hpp"
#include "event.hpp"
#include <map>

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
    void toggle(EventType);
    void dispatchEvent();
    EventType mode_ = EventType::IDLE;
    std::unique_ptr<Event> active_event_;
};

} //namespace app

#endif //FSM_H