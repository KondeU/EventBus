#pragma once

namespace tibus {

template <typename Event>
class BusHandler : public Event {
public:
    ~BusHandler()
    {
    }

protected:
    template <typename Actor>
    inline bool BusConnect(const Actor* actor)
    {
    }

    template <typename Actor>
    inline bool BusDisconnect(const Actor* actor)
    {
    }

private:
    bool isConnected = false;
    void* cachedActor = nullptr;
};

}
