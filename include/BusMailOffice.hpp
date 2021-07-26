#pragma once

#include "communicate/Communicator.hpp"

namespace tibus {

class BusMailOffice final : public common::Singleton<BusMailOffice> {
public:
    BusMailOffice()
    {
        pubLocalHost = communicate::Communicator::GetReference().
            Create<communicate::Publisher>("tcp://127.0.0.1:6620", true);
        subLocalHost = communicate::Communicator::GetReference().
            Create<communicate::Subscriber>("tcp://127.0.0.1:6621");
        subLocalHost->StartReceive(
            std::bind(&BusMailOffice::ReceivePacketState, this),
            std::bind(&BusMailOffice::ReceivePacketFromLocalHost, this));

        const std::string xsub = "inproc://broker_xsub";
        const std::string xpub = "inproc://broker_xpub";
        // Enable the capture and identify the packets that
        // need to be forwarded (LocalBus and HostsBus) to
        // local host broker in the capture process.
        broker = communicate::Communicator::GetReference().
            Create<communicate::Broker>(xsub, xpub, true);
        broker->SetCaptureCallback(std::bind(
            &BusMailOffice::ForwardPacket, this));
    }

    ~BusMailOffice()
    {
        broker->Terminate();
        subLocalHost->StopReceive();
        subLocalHost->WaitReceive();
        subLocalHost->ResetReceive();
    }

    // Setting before adding any mailman and it will change
    // the number of parallel processing of BusMailOffice.
    void ConfigureThreadCount(size_t count)
    {
        threadCount = count;
    }

protected:
    void ForwardPacket(const std::vector<std::string>& frame)
    {
        // frame: | Bus | Function | ArgsData | = 3
        if (frame.size() != 3) {
            return;
        }

        switch (frame[0].back()) {
        default:  // Unknown
        case 'I': // InnerBus
            // No forwarding required.
            break;

        case 'L': // LocalBus
        case 'H': // HostsBus
            if (pubLocalHost) {
                pubLocalHost->Publish(frame);
            }
            break;
        }
    }

    void ReceivePacketFromLocalHost(
        const std::string& envelop, const std::vector<std::string>& contents)
    {

    }

    void ReceivePacketState(bool isSuccessful)
    {
        (void)isSuccessful; // Nothing to do.
    }

private:
    // Pulish to local host broker (router)
    communicate::Publisher* pubLocalHost;
    // Subscribe from local host broker (router)
    communicate::Subscriber* subLocalHost;

    // In-process broker
    communicate::Broker* broker;

    communicate::Publisher* publisher;
    std::vector<communicate::Subscriber*> subscribers;

    std::vector<std::thread> threads;
    size_t threadCount = 1 + std::thread::hardware_concurrency() / 8;
};

}
