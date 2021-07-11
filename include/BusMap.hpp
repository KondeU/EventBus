#pragma once

#include <vector>
#include <typeinfo>
#include "common/Singleton.hpp"

namespace tibus {

// BusMap only stores all the base classes of Bus,
// which can be used for debugging and tracking.
class BusMap : public common::Singleton<BusMap> {
public:
    // decltype(typeid(void).name())
    // --> const char * --> std::string
    using BusName = std::string;
    using BusHash = decltype(typeid(void).hash_code());

    // The BusTrait template class will inherit from
    // BusMap::BusBase, and it will be used to regist
    // to BusMap(store in BusInfo).
    class BusBase {
    public:
        // Some compilers will optimize this empty class
        // without any virtual function and merge it with
        // the subclass, so that dynamic_cast will not be
        // able to convert base class to the child class
        // normally, leaving a default virtual destructor
        // function to avoid this optimization.
        virtual ~BusBase() = default;
    };

    struct BusInfo {
        BusName name;
        BusHash hash;
        BusBase* instance;
    };

    template <typename Bus>
    inline void Regist()
    {
        static_assert(
            std::is_base_of<BusBase, Bus>::value,
            "Bus must derived from BusMap::BusBase!");
        buses.emplace_back({
            typeid(Bus).name(),
            typeid(Bus).hash_code(),
            &Bus::GetReference() });
    }

private:
    std::vector<BusInfo> buses;
};

}
