#pragma once

namespace tibus {

template <typename ...Handlers>
class BusActor : public Handlers... {
public:
    virtual const std::string& UniqueName() const
    {
        return "";
    }

protected:
    BusActor() = default;

    template <typename Bus, typename Event, typename Actor, typename Return, class Args...>
    inline Return EventNow(const Actor& actor, Return(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Bus, typename Event, typename Actor, class Args...>
    inline void EventNow(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Bus, typename Event, class Args...>
    inline void EventNow(void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Bus, typename Event, typename Actor, class Args...>
    inline void Event(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Bus, typename Event, class Args...>
    inline void Event(const std::string& name, void(Event::* func)(Args...), const Args& ...args) const
    {
    }

    template <typename Bus, typename Event, class Args...>
    inline void Event(void(Event::* func)(Args...), const Args& ...args) const
    {
    }
};

}
