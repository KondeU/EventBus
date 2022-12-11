#pragma once

#include "EventBus.hpp"

class LocalProcessBus;

class LocalProcessBusEvent : public au::ebus::BusEvent<LocalProcessBus> {
public:
    virtual void OnLocalProcessTest(long long counter) = 0;
};

class LocalProcessBus : public au::ebus::BusTrait<LocalProcessBus, LocalProcessBusEvent
    , au::ebus::LocalHostBusGroup> {
public:
    void OnInit() override
    {
        DefineBus("LocalProcessBus");
        DefineFunction("OnLocalProcessTest", &LocalProcessBusEvent::OnLocalProcessTest);
    }
};
