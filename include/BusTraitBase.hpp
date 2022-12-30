#pragma once

namespace au {
namespace ebus {

class BusTraitBase {
public:
    virtual void OnInit() {}

protected:
    BusTraitBase() = default;
};

}
}
