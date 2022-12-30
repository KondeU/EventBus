#pragma once

#include "EventBus.hpp"

class InProcessBus;

class InProcessBusEvent : public au::ebus::BusEvent<InProcessBus> {
public:
    virtual int OnInProcessTestReturn(int counter) = 0;
    virtual void OnInProcessTest(int counter, std::string method) = 0;
};

class InProcessBus : public au::ebus::BusTrait<InProcessBus, InProcessBusEvent> {
public:
    void OnInit() override
    {
        DefineBus("InProcessBus");
        DefineFunction("OnInProcessTest", &InProcessBusEvent::OnInProcessTest);
        // Do not define the OnInProcessTestReturn function because
        // the return value is only valid when using EventNow.
    }
};

BUS_REF(InProcessBus);
