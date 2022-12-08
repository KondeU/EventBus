#include "tools/hosts_broker_core.h"
#include "CommunicateContext.hpp"

namespace au {
namespace commtool {

bool HostsBroker::Start(int port1, int port2)
{
    std::string sub = "tcp://*:" + std::to_string(port2);
    std::string pub = "tcp://*:" + std::to_string(port1);
    auto& communicator = communicate::CommunicateContext::GetReference();
    broker = communicator.Create<communicate::Broker>(sub, pub);
    return (broker != nullptr);
}

void HostsBroker::Stop()
{
    if (broker) {
        broker->Terminate(); // Terminating broker thread!
        communicate::CommunicateContext::GetReference().Destroy(broker);
        broker = nullptr;
    }
}

HostsBroker::~HostsBroker()
{
    Stop();
}

}
}
