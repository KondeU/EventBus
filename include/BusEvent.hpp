#pragma once

namespace au {
namespace ebus {

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
}
