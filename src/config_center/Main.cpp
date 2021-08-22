#include <iostream>
#include "TiBus.hpp"

class MyModule : public tibus::BusActor<
    tibus::BusHandler<TickBusEvent>, tibus::BusHandler<ClockBusEvent>> {
public:
    MyModule()
    {
        tibus::BusHandler<TickBusEvent>::BusConnect(this);
        tibus::BusHandler<ClockBusEvent>::BusConnect(this);
    }

    void OnTick() override
    {
        std::cout << "Tick!" << std::endl;
    }

    void OnClock(int id) override
    {
        std::cout << "Tick!" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    TickBus::GetReference().EventQueue(&TickBusEvent::OnTick);

    return 0;
}
