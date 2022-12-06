#pragma once

#include "BusActorBase.hpp"

namespace au {
namespace ebus {

template <typename ...Handlers>
class BusActor : public BusActorBase, public Handlers... {
protected:
    BusActor() = default;

    template <typename Bus, typename Event, typename Actor, typename Return, class ...Args>
    inline Return EventNow(const Actor& actor, Return(Event::* func)(Args...), const Args& ...args) const
    {
        return Bus::GetReference().EventNow(actor, func, args...);
    }

    template <typename Bus, typename Event, typename Actor, class ...Args>
    inline void EventNow(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        Bus::GetReference().EventNow(actor, func, args...);
    }

    template <typename Bus, typename Event, class ...Args>
    inline void EventNow(void(Event::* func)(Args...), const Args& ...args) const
    {
        Bus::GetReference().EventNow(func, args...);
    }

    template <typename Bus, typename Event, typename Actor, class ...Args>
    inline void EventQueue(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        Bus::GetReference().EventQueue(actor, func, args...);
    }

    template <typename Bus, typename Event, class ...Args>
    inline void EventQueue(const std::string& name, void(Event::* func)(Args...), const Args& ...args) const
    {
        Bus::GetReference().EventQueue(name, func, args...);
    }

    template <typename Bus, typename Event, class ...Args>
    inline void EventQueue(void(Event::* func)(Args...), const Args& ...args) const
    {
        Bus::GetReference().EventQueue(func, args...);
    }
};

}
}
