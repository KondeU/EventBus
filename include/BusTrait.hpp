#pragma once

#include "BusManager.hpp"

namespace tibus {

template <typename Bus, typename Event, typename Group = InProcessBusGroup>
class BusTrait : public BusTraitBase, public common::Singleton<Bus> {
public:
    template <typename Actor, typename Return, class Args...>
    Return EventNow(const Actor& actor, Return(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <typename Actor, class Args...>
    void EventNow(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <class Args...>
    void EventNow(void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <typename Actor, class Args...>
    void EventQueue(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <class Args...>
    void EventQueue(const std::string& name, void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <class Args...>
    void EventQueue(void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

protected:
    BusTrait()
    {
        BusManager::GetReference().RegistBus(*this);
    }

    void DefineBus(const std::string& name) const
    {
        // TODO
    }

    template <class ...Args>
    void DefineFunction(const std::string& name, void(Event::* func)(Args...)) const
    {
        // TODO
    }

private:
    friend class BusHandler<Event>;
    std::unordered_map<void*, Event*> handlers;
    std::unordered_map<std::string, Event*> mapping; // TODO?
};

}
