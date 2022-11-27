#include <csignal>
#include <iostream>

#include "TiBus.hpp"

class Application : public ApplicationFramework
    , public tibus::common::Singleton<Application> {
};

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        Application::GetReference().Exit();
    }
}

class MyModule : public tibus::BusActor<
    tibus::BusHandler<TickBusEvent>/*, tibus::BusHandler<ClockBusEvent>*/> {
public:
    MyModule()
    {
        tibus::BusHandler<TickBusEvent>::BusConnect(*this);
        //tibus::BusHandler<ClockBusEvent>::BusConnect(this);
        //EventNow<ClockBus>(&ClockBusEvent::OnClock, 1);
    }

    void OnTick() override
    {
        std::cout << "Tick!" << std::endl;
    }

    //void OnClock(int id) override
    //{
    //    std::cout << "Clock!" << std::endl;
    //}
};

int main(int argc, char* argv[])
{
    MyModule myModule; // for test

    signal(SIGINT, SignalHandler);
    Application::GetReference().Configure(argc, argv);
    Application::GetReference().Run();
    return 0;
}
