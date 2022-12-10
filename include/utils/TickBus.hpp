#pragma once

#include "EventBus.hpp"

namespace au {

class TickBus;

class TickBusEvent : public ebus::BusEvent<TickBus> {
public:
    virtual void OnTick() = 0;
};

class TickBus : public ebus::BusTrait<TickBus, TickBusEvent> {
public:
    void OnInit() override
    {
        DefineBus("TickBus");
        DefineFunction("OnTick", &TickBusEvent::OnTick);
    }
};

}
