#pragma once

#include <zmq.hpp>

namespace ti {
namespace communicate {

class Publisher {
public:
    void Publish(const std::string& envelope, const std::string& content)
    {
        // envelope value must be a pure string.
        socket.send(zmq::buffer(envelope), zmq::send_flags::sndmore);
        socket.send(zmq::buffer(content, content.size()), zmq::send_flags::none);
    }

private:
    friend class Communicator;

    explicit Publisher(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::pub)
    {
    }

    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //       x.x.x.x:x means ip:port
        try {
            socket.bind("tcp://" + addr);
        } catch (zmq::error_t) {
            // IP or port is incorrect,
            // or the port is occupied.
            return false;
        }
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using PublisherInst = Publisher*;

}
}
