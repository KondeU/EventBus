#pragma once

#include "BusGroup.hpp"
#include "BusDebugger.hpp"

namespace au {
namespace ebus {

class BusManager : public common::GlobalSingleton<BusManager> {
public:
    enum class BusGroupLevel {
        InProcess,
        LocalHost,
        MultiHost
    };

    struct BusInfo {
        struct LocalBrokerInfo {
            int sport = 0; // subscriber port
            int pport = 0; // publisher port
            LocalBrokerInfo(int sp, int pp)
                : sport(sp), pport(pp) {}
            LocalBrokerInfo() {}
        };
        LocalBrokerInfo l;

        struct HostsBrokerInfo {
            std::string ip;
            int sport = 0; // subscriber port
            int pport = 0; // publisher port
            HostsBrokerInfo(std::string ip, int sp, int pp)
                : ip(ip), sport(sp), pport(pp) {}
            HostsBrokerInfo() {}
        };
        // Because the broker has a single point of failure,
        // I plan to support the multi brokers in the future.
        std::vector<HostsBrokerInfo> h;
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

    bool Start(BusGroupLevel level, const BusInfo& info) const
    {
        if (working) {
            return false;
        }

        switch (level) {
        case BusGroupLevel::MultiHost:
            if (info.h.size() > 0) {
                MultiHostBusGroup::GetReference().Start(
                    info.h[0].ip, info.h[0].sport, info.h[0].pport);
            }
            // FALL THROUGH
        case BusGroupLevel::LocalHost:
            LocalHostBusGroup::GetReference().Start(
                info.l.sport, info.l.pport);
            // FALL THROUGH
        case BusGroupLevel::InProcess:
            break; // Nothing to do.
        }

        for (auto bus : buses) {
            bus->OnInit();
        }

        working = true;
        return true;
    }

    bool Stop() const
    {
        if (!working) {
            return false;
        }

        MultiHostBusGroup::GetReference().Stop();
        LocalHostBusGroup::GetReference().Stop();

        working = false;
        return true;
    }

    void ExecuteReceivedEvents() const
    {
        BusDebugger::GetReference().ThreadCheckAssert();
        InProcessBusGroup::GetReference().Update();
        LocalHostBusGroup::GetReference().Update();
        MultiHostBusGroup::GetReference().Update();
    }

private:
    std::vector<BusTraitBase*> buses;
    mutable bool working = false;
};

}
}
