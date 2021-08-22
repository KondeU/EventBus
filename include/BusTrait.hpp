#pragma once

#include <string>
#include <unordered_map>

namespace tibus {

class BusTraitBase {
public:
    virtual void OnInit() {}

protected:
    BusTraitBase() = default;
};

template <typename Bus, typename Event, typename Group = InProcessBusGroup>
class BusTrait : public BusTraitBase, public Singleton<Bus> {
public:
    template <typename Actor, typename Return, class Args...>
    Return EventNow(const Actor& actor, Return(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Actor, class Args...>
    void EventNow(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <class Args...>
    void EventNow(void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Actor, class Args...>
    void Event(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <class Args...>
    void Event(const std::string& name, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <class Args...>
    void Event(void(Event::* func)(Args...), const Args& ...args) const
    {
    }

protected:
    BusTrait()
    {
    }

    void DefineBus(const std::string& name) const
    {
    }

    template <class ...Args>
    void DefineFunction(const std::string& name, void(Event::* func)(Args...)) const
    {
    }

private:
    friend class BusHandler<Event>;
    std::unordered_map<void*, Event*> handlers;
    std::unordered_map<std::string, Event*> mapping;
};

}
