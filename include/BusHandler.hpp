#pragma once

namespace tibus {

template <typename Event>
class BusHandler : public Event {
public:
    ~BusHandler()
    {
        if (cached != nullptr) {
            BusDisconnect(cached);
        }
    }

protected:
    inline bool BusConnect(BusActorBase* actor)
    {
        if (cached != nullptr) {
            return false;
        }
        if (actor == nullptr) {
            return false;
        }

        if (!(GetRelatedBus().handlers.emplace(actor, this)).second) {
            return false;
        }

        if (actor.UniqueName() != "") {
            if (!(GetRelatedBus().namedHandlers.emplace(
                actor->UniqueName(), this)).second) {
                GetRelatedBus().handlers.erase(actor);
                return false;
            }
        }

        cached = actor;
        return true;
    }

    inline bool BusDisconnect(BusActorBase* actor)
    {
        if (cached == nullptr) {
            return false;
        }
        if (cached != actor) {
            return false;
        }

        if (GetRelatedBus().handlers.erase(actor) <= 0) {
            return false;
        }

        if (actor.UniqueName() != "") {
            if (GetRelatedBus().handlers.erase(actor) <= 0) {
                return false;
            }
        }

        cached = nullptr;
        return true;
    }

private:
    BusActorBase* cached = nullptr;
};

}
