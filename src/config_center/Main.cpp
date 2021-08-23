#include <csignal>
#include <iostream>
#include "TiBus.hpp"

bool g_loop = true;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_loop = false;
    }
}

class MyModule : public tibus::BusActor<
    tibus::BusHandler<TickBusEvent>, tibus::BusHandler<ClockBusEvent>> {
public:
    MyModule()
    {
        tibus::BusHandler<TickBusEvent>::BusConnect(this);
        tibus::BusHandler<ClockBusEvent>::BusConnect(this);
        EventNow<ClockBus>(&ClockBusEvent::OnClock, 1);
    }

    void OnTick() override
    {
        std::cout << "Tick!" << std::endl;
    }

    void OnClock(int id) override
    {
        std::cout << "Clock!" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);

    tibus::BusManager::GetReference().Start();

    MyModule myModule;

    while (g_loop) {
        TickBus::GetReference().EventQueue(&TickBusEvent::OnTick);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
