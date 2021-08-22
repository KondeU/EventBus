#include <iostream>
#include "TiBus.hpp"

class MyModule : public tibus::BusActor<tibus::BusHandler<TickBusEvent>> {
public:
    MyModule()
    {
        //tibus::BusActor<tibus::BusHandler<TickBusEvent>>::BusConnect(this);
    }

    void OnTick(int i) override
    {
        std::cout << "Tick!" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    TickBus::GetReference().EventQueue(&TickBusEvent::OnTick, 1);

    return 0;
}
