#pragma once

#include <vector>
#include "BusMap.hpp"
#include "BusMailbox.hpp"

namespace tibus {

template <typename Bus, typename Event>
class BusTrait : public BusMap::BusBase, public common::Singleton<Bus> {
protected:
    BusTrait()
    {
        Regist<Bus>();
    }

    void DefineTopic(const std::string& topic, BusPolicy policy)
    {
        mailbox.BindTopic(topic, policy);
    }

    template <class ...Args>
    void DefineFunction(const std::string& name, void(Event::*func)(Args...))
    {
        // For BusMailbox, this function is the mail processing function.
        auto proxy = [this, func](Args ...args) {
            for (Event* handler : handlers) {
                (handler->*func)(args...);
            }
        };
        mailbox.BindFunction(name, std::function<void(Args...)>(proxy));
    }

private:
    BusMailbox mailbox;

    // The addition and deletion of `handlers`
    // are handled in the Connect and Disconnect
    // functions of BusHandler, see BusHandler.hpp
    friend class BusHandler<Event>;
    std::vector<Event*> handlers;
};

}
