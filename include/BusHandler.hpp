#pragma once

#include "BusActorBase.hpp"

namespace au {

template <typename Event>
class BusHandler : public Event {
public:
    ~BusHandler()
    {
        if (cached != nullptr) {
            BusDisconnect(*cached);
        }
    }

protected:
    inline bool BusConnect(const BusActorBase& actor)
    {
        if (cached != nullptr) {
            return false;
        }

        if (!(Event::GetRelatedBus().handlers.emplace(&actor, this)).second) {
            return false;
        }

        if (actor.UniqueName() != "") {
            if (!(Event::GetRelatedBus().namedHandlers.emplace(
                actor.UniqueName(), this)).second) {
                Event::GetRelatedBus().handlers.erase(&actor);
                return false;
            }
        }

        cached = &actor;
        return true;
    }

    inline bool BusDisconnect(const BusActorBase& actor)
    {
        if ((cached == nullptr) || (cached != &actor)) {
            return false;
        }

        if (Event::GetRelatedBus().handlers.erase(&actor) <= 0) {
            return false;
        }

        if (actor.UniqueName() != "") {
            if (Event::GetRelatedBus().handlers.erase(&actor) <= 0) {
                return false;
            }
        }

        cached = nullptr;
        return true;
    }

private:
    const BusActorBase* cached = nullptr;
};

}
