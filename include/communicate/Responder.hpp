#pragma once

#include <thread>
#include <unordered_map>
#include "Communicator.hpp"

namespace tibus {
namespace communicate {

class Responder : public Communicator {
public:
    bool StartResponse(const std::function<void(const std::string&, std::string&)>& proc)
    {
        if (responds.find(respKey) != responds.end()) {
            return false;
        }

        responds.emplace(respKey, std::make_pair(std::thread([this, &proc]() {
            while (responds.find(respKey) == responds.end()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } // Spin lock until emplaced.

            bool& exit = responds.find(respKey)->second.second;
            while (exit) {
                zmq::message_t reqMsg;
                (void)socket.recv(reqMsg);

                if (reqMsg.size() > 0) {
                    std::string request;
                    std::string respond;

                    request = request.replace(request.begin(), request.end(),
                        static_cast<char*>(reqMsg.data()), reqMsg.size());

                    proc(request, respond);

                    zmq::message_t respMsg(respond.data(), respond.size());
                    socket.send(respMsg, zmq::send_flags::none);
                }
            }
        }), true));

        return true;
    }

    bool StopResponse()
    {
        auto respIter = responds.find(respKey);
        if (respIter == responds.end()) {
            return false;
        }
        respIter->second.second = false;
        return true;
    }

    bool WaitResponse()
    {
        auto respIter = responds.find(respKey);
        if (respIter == responds.end()) {
            return false;
        }
        if (respIter->second.first.joinable()) {
            respIter->second.first.join();
        }
        return true;
    }

    bool ResetResponse()
    {
        auto respIter = responds.find(respKey);
        if (respIter == responds.end()) {
            return false;
        }
        responds.erase(respIter);
        return true;
    }

private:
    friend class CommunicateContext;

    explicit Responder(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::rep)
    {
    }

    bool Init(const std::string& address)
    {
        try {
            socket.bind(address);
        } catch (zmq::error_t) {
            return false;
        }
        // Responder blocks and calls recv per second.
        socket.set(zmq::sockopt::rcvtimeo, 1000);
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;

    std::unordered_map<std::string, std::pair<std::thread, bool>> responds;
    const std::string respKey = "respond";
};

}
}
