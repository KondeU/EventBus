#pragma once

namespace tibus {

template <typename Bus>
class BusEvent {
protected:
    inline Bus& RelatedBus()
    {
        return Bus::GetReference();
    }
};

}
