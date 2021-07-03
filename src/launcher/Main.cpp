// FIXME: Temporary test used!

#include <csignal>
#include <iostream>
#include "communicate/Communicator.hpp"
using namespace tibus::communicate;

bool g_loop = true;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_loop = false;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);

    auto broker = Communicator::GetReference().Create<Broker>(
        "tcp://*:6018", "tcp://*:6019", false);

    auto publisher = Communicator::GetReference().Create<Publisher>(
        "tcp://127.0.0.1:6018", true);
    auto subscriber = Communicator::GetReference().Create<Subscriber>(
        "tcp://127.0.0.1:6019");
    subscriber->Subscribe("");

    std::function<void(bool)> callback = [](bool) {};
    std::function<void(const std::string&, const std::string&)> process =
    [](const std::string& envelop, const std::string& content) {
        std::cout << ("[subscriber] " + envelop + ", " + content + "\r\n");
    };
    subscriber->StartReceive(callback, process);

    broker->SetCaptureCallback([](std::vector<std::string> messages) {
        size_t index = 0;
        for (auto& each : messages) {
            std::cout << ("[capture](" + std::to_string(index)
                + ") " + each + "\r\n");
            index++;
        }
    });

    unsigned int index = 0;
    while (g_loop) {
        std::string envelop = "c" + std::to_string(index % 3);
        std::string content = "CrntIdx is " + std::to_string(index);
        publisher->Publish(envelop, content);
        std::cout << ("[publisher] " + envelop + ", " + content + "\r\n");
        if (index % 10 == 0) {
            if ((index / 10) % 2) {
                broker->Pause();
            } else {
                broker->Resume();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        index++;
    }

    broker->Terminate();

    subscriber->StopReceive();
    subscriber->WaitReceive();
    subscriber->ResetReceive();

    return 0;
}

