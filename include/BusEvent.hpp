#pragma once

namespace tibus {

template <typename Bus>
class BusEvent {
protected:
    BusEvent() = default;

    inline Bus& GetRelatedBus() const
    {
    }
};

}
