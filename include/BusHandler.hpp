#pragma once

#include "BusTrait.hpp"

namespace tibus {

template <typename Event>
class BusHandler : public Event {
protected:
    void Connect()
    {
        Bus& bus = RelatedBus();
        bus.handlers.emplace_back(this);
    }

    void Disconnect()
    {
        Bus& bus = RelatedBus();
        std::vector<Event*>::iterator it = std::find(
            bus.handlers.begin(), bus.handlers.end(), this);
        if (it != bus.handlers.end()) {
            bus.handlers.erase(it);
        }
    }
};

}
