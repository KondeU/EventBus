#pragma once

#include <thread>
#include <zmq_addon.hpp>
#include "Communicator.hpp"

namespace tibus {
namespace communicate {

class Subscriber : public Communicator {
public:
    void Subscribe(const std::string& envelope)
    {
        if (topics.emplace(envelope).second) {
            socket.set(zmq::sockopt::subscribe, envelope);
        }
    }

    void Unsubscribe(const std::string& envelope)
    {
        if (topics.erase(envelope) > 0) {
            socket.set(zmq::sockopt::unsubscribe, envelope);
        }
    }

    bool StartReceive(std::function<void(bool)> callback, // bool: is current timeslice received?
        std::function<void(const std::string&, const std::vector<std::string>&)> process)
    {
        if (subscribes.find(subKey) != subscribes.end()) {
            return false;
        }

        subscribes.emplace(subKey, std::make_pair(std::thread(
        [this, callback, process]() {
            while (subscribes.find(subKey) == subscribes.end()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } // Spin lock until emplaced.

            bool& exit = subscribes.find(subKey)->second.second;
            while (exit) {
                std::vector<zmq::message_t> messages;
                (void)zmq::recv_multipart(socket, std::back_inserter(messages));
                if (messages.size() > 0) { // envelope + contents...
                    std::string envelope;  // envelope value is a pure string.
                    std::vector<std::string> contents;
                    envelope = messages[0].to_string();
                    contents.resize(messages.size() - 1);
                    for (size_t n = 1; n < messages.size(); n++) {
                        std::string& content = contents[n - 1];
                        content = content.replace(content.begin(), content.end(),
                            static_cast<char*>(messages[n].data()), messages[n].size());
                    }
                    process(envelope, contents);
                    callback(true);
                } else {
                    callback(false); // false means receive timeout!
                }
            }
        }), true));

        return true;
    }

    bool StopReceive()
    {
        auto subIter = subscribes.find(subKey);
        if (subIter == subscribes.end()) {
            return false;
        }
        subIter->second.second = false;
        return true;
    }

    bool WaitReceive()
    {
        auto subIter = subscribes.find(subKey);
        if (subIter == subscribes.end()) {
            return false;
        }
        if (subIter->second.first.joinable()) {
            subIter->second.first.join();
        }
        return true;
    }

    bool ResetReceive()
    {
        auto subIter = subscribes.find(subKey);
        if (subIter == subscribes.end()) {
            return false;
        }
        subscribes.erase(subIter);
        return true;
    }

private:
    friend class CommunicateContext;

    explicit Subscriber(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::sub)
    {
    }

    bool Init(const std::string& address, bool reverse = false,
        int hwm = 0, std::vector<std::string> encryption = {})
    {
        // Subscriber blocks and calls recv_multipart per second.
        socket.set(zmq::sockopt::rcvtimeo, 1000);
        // Set a High-Water Mark for the receiver, default is no limit.
        socket.set(zmq::sockopt::rcvhwm, hwm);
        // Setup encryption.
        SetupEncryption(socket, encryption);
        try {
            if (reverse) {
                socket.bind(address);
            } else {
                socket.connect(address); // Standard subscriber
            }
        } catch (zmq::error_t) {
            return false;
        }
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;

    std::unordered_set<std::string> topics;

    std::unordered_map<std::string, std::pair<std::thread, bool>> subscribes;
    const std::string subKey = "subscribe";
};

}
}
