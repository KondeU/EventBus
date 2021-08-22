#pragma once

#include "../TiBus.hpp"

class TickBus;

class TickBusEvent : public tibus::BusEvent<TickBus> {
public:
    virtual void OnTick() = 0;
};

class TickBus : public tibus::BusTrait<TickBus, TickBusEvent> {
public:
    void OnInit() override
    {
        DefineBus("TickBus");
        DefineFunction("OnTick", &TickBusEvent::OnTick);
    }
};
