#pragma once

#include <zmq.hpp>
#include "CommunicationCode.hpp"

namespace ti {
namespace communicate {

class Requester {
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
    friend class Communicator;

    explicit Requester(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::req)
    {
    }

    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //      x.x.x.x:x means ip:port
        // Requester is Req/Rep model's Client.
        try {
            socket.connect("tcp://" + addr);
        } catch (zmq::error_t) {
            // IP or port is incorrect.
            return false;
        }
        // Set the buffer of the client to 0 in order to
        // make sure that messages will not accumulate.
        socket.set(zmq::sockopt::linger, 0);
        if (socket.get(zmq::sockopt::linger) != 0) {
            return false;
        }
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using RequesterInst = Requester*;

}
}
