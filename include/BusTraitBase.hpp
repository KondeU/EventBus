#pragma once

namespace tibus {

class BusTraitBase {
public:
    virtual void OnInit() {}

protected:
    BusTraitBase() = default;
};

}
