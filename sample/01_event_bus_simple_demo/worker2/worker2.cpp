#include <csignal>
#include "../bus_def/LocalProcessBus.h"
#include "../bus_def/HostsProcessBus.h"

class Application : public au::ApplicationFramework
    , public au::common::Singleton<Application> {
public:
    bool OnStart() override
    {
        auto& communicator = au::communicate::CommunicateContext::GetReference();
        unsigned int systemThreadCount = std::thread::hardware_concurrency();
        communicator.Configure(systemThreadCount / 2, 64);
        return true;
    }
};

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        Application::GetReference().Exit();
    }
}

class WorkerTest_Local : public au::ebus::BusActor<au::ebus::BusHandler<LocalProcessBusEvent>> {
public:
    WorkerTest_Local()
    {
        BusConnect(*this);
    }

    void OnLocalProcessTest(long long counter) override
    {
        std::cout << "OnLocalProcessTest called. arg is: " << counter << std::endl;
    }
};

class WorkerTest_Hosts : public au::ebus::BusActor<au::ebus::BusHandler<HostsProcessBusEvent>> {
public:
    WorkerTest_Hosts()
    {
        BusConnect(*this);
    }

    void OnHostsProcessTest(int counterIndex, std::string counterText) override
    {
        assert(std::to_string(counterIndex) == counterText);
        std::cout << "OnHostsProcessTest called. arg is: " << counterIndex << std::endl;
    }

    std::string UniqueName() const override
    {
        return "HandlerDemo";
    }
};

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);
    WorkerTest_Local testLocal;
    WorkerTest_Hosts testHosts;
    Application::GetReference().Configure("level=H_B");
    Application::GetReference().Configure(argc, argv);
    Application::GetReference().Run();
    return 0;
}
