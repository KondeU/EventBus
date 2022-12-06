#pragma once

namespace au {

template <typename Bus>
class BusEvent {
protected:
    BusEvent() = default;

    inline Bus& GetRelatedBus() const
    {
        return Bus::GetReference();
    }
};

}
