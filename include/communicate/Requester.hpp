#pragma once

#include "Communicator.hpp"
#include "CommunicationCode.hpp"

namespace tibus {
namespace communicate {

class Requester : public Communicator {
public:
    int SetTimeout(int ms)
    {
        socket.set(zmq::sockopt::rcvtimeo, ms);
        return socket.get(zmq::sockopt::rcvtimeo);
    }

    CommunicationCode Request(const std::string& request, std::string& respond)
    {
        zmq::message_t reqMsg(request.data(), request.size());
        socket.send(reqMsg, zmq::send_flags::none);

        zmq::message_t respMsg;
        (void)socket.recv(respMsg);

        if (respMsg.size() <= 0) {
            return CommunicationCode::ReceiveTimeout;
        }

        respond = respond.replace(respond.begin(), respond.end(),
            static_cast<char*>(respMsg.data()), respMsg.size());

        return CommunicationCode::Success;
    }

private:
    friend class CommunicateContext;

    explicit Requester(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::req)
    {
    }

    bool Init(const std::string& address)
    {
        // Set the buffer of the client to 0 in order to
        // make sure that messages will not accumulate.
        socket.set(zmq::sockopt::linger, 0);
        try {
            socket.connect(address);
        } catch (zmq::error_t) {
            return false;
        }
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};

}
}
