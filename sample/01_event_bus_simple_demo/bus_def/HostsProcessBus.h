#pragma once

#include "EventBus.hpp"

class HostsProcessBus;

class HostsProcessBusEvent : public au::ebus::BusEvent<HostsProcessBus> {
public:
    virtual void OnHostsProcessTest(int counterIndex, std::string counterText) = 0;
};

class HostsProcessBus : public au::ebus::BusTrait<HostsProcessBus, HostsProcessBusEvent
    , au::ebus::MultiHostBusGroup> {
public:
    void OnInit() override
    {
        DefineBus("HostsProcessBus");
        DefineFunction("OnHostsProcessTest", &HostsProcessBusEvent::OnHostsProcessTest);
    }
};
