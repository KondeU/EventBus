#pragma once

#include "BusManager.hpp"
#include "BusHandler.hpp"

namespace tibus {

template <typename Bus, typename Event, typename Group = InProcessBusGroup>
class BusTrait : public BusTraitBase, public common::GlobalSingleton<Bus> {
public:
    template <typename Actor, typename Return, class ...Args>
    Return EventNow(const Actor& actor, Return(Event::* func)(Args...), const Args& ...args) const
    {
        const auto& handler = handlers.find(&actor);
        if (handler == handlers.end()) {
            return {}; // User logic error, actor not found!
        }
        return (handler.second->*func)(args...);
    }

    template <typename Actor, class ...Args>
    void EventNow(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        const auto& handler = handlers.find(&actor);
        if (handler == handlers.end()) {
            return; // User logic error, actor not found!
        }
        (handler.second->*func)(args...);
    }

    template <class ...Args>
    void EventNow(void(Event::* func)(Args...), const Args& ...args) const
    {
        for (const auto& handler : handlers) {
            (handler.second->*func)(args...);
        }
    }

    template <typename Actor, class ...Args>
    void EventQueue(const Actor& actor, void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <class ...Args>
    void EventQueue(const std::string& name, void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

    template <class ...Args>
    void EventQueue(void(Event::* func)(Args...), const Args& ...args) const
    {
        // TODO
    }

protected:
    BusTrait()
    {
        BusManager::GetReference().RegistBus(*this);
    }

    void DefineBus(const std::string& name)
    {
        busName = name;
        Group::GetReference().BindBus(name, *this);
    }

    template <class ...Args>
    void DefineFunction(const std::string& name, void(Event::* func)(Args...))
    {
        functionsName[reinterpret_cast<uintptr_t*&>(func)] = name;
        auto proxy = [this, func](std::string id, Args ...args) {
            if (id.empty()) {
                for (const auto& handler : handlers) {
                    (handler.second->*func)(args...);
                }
            } else {
                const auto& handler = namedHandlers.find(id);
                if (handler != namedHandlers.end()) {
                    (handler->second->*func)(args...);
                }
            }
        };
        Group::GetReference().BindFunction(name, *this,
            std::function<void(std::string, Args...)>(proxy));
    }

private:
    friend class BusHandler<Event>;
    std::unordered_map<std::string, Event*> namedHandlers;
    std::unordered_map<const BusActorBase*, Event*> handlers;

    // Stores the reflect information for calling EventQueue
    std::string busName;                                       // *this <-> busName
    std::unordered_map<uintptr_t*, std::string> functionsName; // _vptr <-> functionsName
};

}
