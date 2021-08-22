#pragma once

#include "BusGroup.hpp"

namespace tibus {

class BusManager : public common::Singleton<BusManager> {
public:
    enum class BusGroupLevel {
        InProcess,
        LocalHost,
        MultiHost
    };

    ~BusManager()
    {
        if (working) {
            Stop();
        }
    }

    void RegistBus(BusTraitBase& bus)
    {
        buses.emplace_back(&bus);
    }

    bool Start(BusGroupLevel level = BusGroupLevel::InProcess) const
    {
        if (working) {
            return false;
        }

        // TODO

        for (auto bus : buses) {
            bus->OnInit();
        }

        // TODO
        return true;
    }

    bool Stop() const
    {
        if (!working) {
            return false;
        }

        // TODO

        return true;
    }

    void ExecuteReceivedEvents() const
    {
        // TODO
    }

private:
    std::vector<BusTraitBase*> buses;
    bool working = false;
};

}
