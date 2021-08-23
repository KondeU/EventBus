#pragma once

namespace tibus {

template <typename Event>
class BusHandler : public Event {
public:
    ~BusHandler()
    {
        if (cached != nullptr) {
            BusDisconnect(static_cast<BusActorBase*>(cached));
        }
    }

protected:
    template <typename Actor>
    inline bool BusConnect(Actor* actor)
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
        // TODO

        cached = actor;
        return true;
    }

    template <typename Actor>
    inline bool BusDisconnect(Actor* actor)
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
        // TODO

        cached = nullptr;
        return true;
    }

private:
    void* cached = nullptr; // cached actor
};

}
