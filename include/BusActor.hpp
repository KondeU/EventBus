#pragma once

namespace tibus {

template <typename ...Handlers>
class BusActor : public Handlers... {
public:
    // TODO: Event
};

}
