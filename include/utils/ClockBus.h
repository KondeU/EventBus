#pragma once

#include "../TiBus.hpp"

class ClockBus;

class ClockBusEvent : public tibus::BusEvent<ClockBus> {
public:
    virtual void OnClock(int id) = 0;
};

class ClockBus : public tibus::BusTrait<ClockBus, ClockBusEvent> {
public:
    void OnInit() override
    {
        DefineBus("ClockBus");
        DefineFunction("OnClock", &ClockBusEvent::OnClock);
    }

    bool CreateTimer(int id, double ms)
    {
    }

    bool DestroyTimer(int id)
    {
    }

private:
    class Clock {
    };

    std::unordered_map<int, Clock> clocks;
};
