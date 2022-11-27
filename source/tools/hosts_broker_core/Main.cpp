#include <csignal>
#include "TiBus.hpp"

class Application : public ApplicationFramework
    , public tibus::common::Singleton<Application> {
public:
    bool OnStart() override
    {
        auto& communicator = tibus::communicate::CommunicateContext::GetReference();
        unsigned int systemThreadCount = std::thread::hardware_concurrency();
        communicator.Configure(systemThreadCount / 2, 64);

        std::string sub = "tcp://*:" + std::to_string(GetConfigInt("hbp2"));
        std::string pub = "tcp://*:" + std::to_string(GetConfigInt("hbp1"));
        broker = communicator.Create<tibus::communicate::Broker>(sub, pub);
        if (!broker) {
            return false;
        }
        return true;
    }

    void OnExit() override
    {
        ExitBroker();
    }

    ~Application()
    {
        ExitBroker();
    }

private:
    void ExitBroker()
    {
        if (broker) {
            broker->Terminate(); // Terminating broker thread!
            tibus::communicate::CommunicateContext::GetReference().Destroy(broker);
            broker = nullptr;
        }
    }

    tibus::communicate::Broker* broker = nullptr;
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
    Application::GetReference().Configure(argc, argv);
    Application::GetReference().Run();
    return 0;
}
