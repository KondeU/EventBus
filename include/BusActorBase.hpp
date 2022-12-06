#pragma once

#include <string>

namespace au {

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
