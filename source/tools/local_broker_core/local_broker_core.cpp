#include "tools/local_broker_core.h"
#include "CommunicateContext.hpp"

namespace au {
namespace commtool {

bool LocalBroker::Start(int port1, int port2)
{
    std::string sub = "tcp://127.0.0.1:" + std::to_string(port2);
    std::string pub = "tcp://127.0.0.1:" + std::to_string(port1);
    auto& communicator = communicate::CommunicateContext::GetReference();
    broker = communicator.Create<communicate::Broker>(sub, pub);
    return (broker != nullptr);
}

void LocalBroker::Stop()
{
    if (broker) {
        broker->Terminate(); // Terminating broker thread!
        communicate::CommunicateContext::GetReference().Destroy(broker);
        broker = nullptr;
    }
}

LocalBroker::~LocalBroker()
{
    Stop();
}

}
}
