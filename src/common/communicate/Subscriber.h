#pragma once

#include <zmq_addon.hpp>

namespace ti {
namespace communicate {

class Subscriber {
public:
    int SetTimeout(int ms)
    {
        socket.set(zmq::sockopt::rcvtimeo, ms);
        return socket.get(zmq::sockopt::rcvtimeo);
    }

    void Subscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::subscribe, envelope);
    }

    void Unsubscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::unsubscribe, envelope);
    }

    bool StartReceive(const std::function<void(bool)>& callback, // bool: receive successfully?
        const std::function<void(const std::string&, const std::string&)>& process)
    {
        if (subscribes.find(subKey) != subscribes.end()) {
            return false;
        }

        subscribes.emplace(subKey, std::make_pair(std::thread(
        [this, &callback, &process]() {
            while (subscribes.find(subKey) == subscribes.end()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } // Spin lock until emplaced

            bool& exit = subscribes.find(subKey)->second.second;
            while (exit) {
                std::vector<zmq::message_t> messages;
                (void)zmq::recv_multipart(socket, std::back_inserter(messages));
                if (messages.size() != 2) { // envelope + content
                    callback(false); // false means receive timeout!
                } else {
                    std::string envelope;
                    std::string content;

                    // envelope value is a pure string.
                    envelope = messages[0].to_string();
                    content = content.replace(content.begin(), content.end(),
                        static_cast<char*>(messages[1].data()), messages[1].size());
                    process(envelope, content);
                    callback(true);
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
    friend class Communicator;

    explicit Subscriber(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::sub)
    {
    }

    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //       x.x.x.x:x means ip:port
        try {
            socket.connect("tcp://" + addr);
        } catch (zmq::error_t) {
            // IP or port is incorrect.
            return false;
        }
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;

    std::unordered_map<std::string, std::pair<std::thread, bool>> subscribes;
    const std::string subKey = "subscribe";
};
using SubscriberInst = Subscriber*;

}
}
