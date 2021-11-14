#include <csignal>
//#include <iostream>
#include "TiBus.hpp"

class Application : public ApplicationFramework
    , public tibus::common::Singleton<Application> {
public:

};

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        Application::GetReference().Exit();
    }
}

class LocalBrokerModule : public tibus::BusActor<
    tibus::BusHandler<TickBusEvent>> {
public:
    LocalBrokerModule()
    {
        BusConnect(*this);
    }

    void OnTick() override
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }
};

int main(int argc, char* argv[])
{
    LocalBrokerModule module;
    signal(SIGINT, SignalHandler);
    Application::GetReference().Run();
    return 0;
}
