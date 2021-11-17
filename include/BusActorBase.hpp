#pragma once

#include <string>

namespace tibus {

class BusActorBase {
public:
    virtual std::string UniqueName() const
    {
        return "";
    }

protected:
    BusActorBase() = default;
};

}
