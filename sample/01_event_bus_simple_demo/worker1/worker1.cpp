#include <csignal>
#include "../bus_def/InProcessBus.h"
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

class WorkerTest : public au::ebus::BusActor<
    au::ebus::BusHandler<au::TickBusEvent>,
    au::ebus::BusHandler<InProcessBusEvent>> {
public:
    WorkerTest()
    {
        au::ebus::BusHandler<au::TickBusEvent>::BusConnect(*this);
        au::ebus::BusHandler<InProcessBusEvent>::BusConnect(*this);
    }

    void OnTick() override
    {
        std::cout << "Tick!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        static int counter = 0;

        int returnValue = InProcessBus::GetReference().EventNow(
            *this, // actor is `*this`, it means WorkerTest instance itself.
            &InProcessBusEvent::OnInProcessTestReturn, counter);
        assert(returnValue == counter);
        InProcessBus::GetReference().EventNow(
            &InProcessBusEvent::OnInProcessTest, counter, std::string("EventNow"));
        InProcessBus::GetReference().EventQueue(
            &InProcessBusEvent::OnInProcessTest, counter, std::string("EventQueue"));

        LocalProcessBus::GetReference().EventQueue(
            &LocalProcessBusEvent::OnLocalProcessTest, long long(counter));

        HostsProcessBus::GetReference().EventQueue("HandlerDemo", // HandlerDemo see worker2.cpp
            &HostsProcessBusEvent::OnHostsProcessTest, counter, std::to_string(counter));

        counter++;
    }

    int OnInProcessTestReturn(int counter) override
    {
        std::cout << "OnInProcessTestReturn called. arg and ret is: " << counter << std::endl;
        return counter;
    }

    void OnInProcessTest(int counter, std::string method) override
    {
        std::cout << "OnInProcessTest called. arg is: " << counter
                  << ", method is: " << method << std::endl;
    }
};

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);
    WorkerTest test;
    Application::GetReference().Configure("level=H_B");
    Application::GetReference().Configure(argc, argv);
    Application::GetReference().Run();
    return 0;
}
