#pragma once

#include <string>

namespace tibus {

class BusActorBase {
public:
    virtual const std::string UniqueName() const
    {
        return "";
    }

protected:
    BusActorBase() = default;
};

}
