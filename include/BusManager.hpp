#pragma once

namespace tibus {

class BusManager : public Singleton<BusManager> {
public:
    enum class BusGroupLevel {
        InProcess,
        LocalHost,
        MultiHost
    };

    ~BusManager()
    {
    }

    void RegistBus(const BusTraitBase& bus)
    {
    }

    bool Start(BusGroupLevel level = BusGroupLevel::InProcess) const
    {
    }

    bool Stop() const
    {
    }

    void ExecuteReceivedEvents() const
    {
    }

private:
    std::vector<BusTraitBase*> buses;
};

}
