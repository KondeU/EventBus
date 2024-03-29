#pragma once

#include <string>

namespace au {
namespace ebus {

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
}
