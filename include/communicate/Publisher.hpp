#pragma once

#include <zmq.hpp>

namespace tibus {
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

    bool Init(const std::string& address, bool proxy = false)
    {
        try {
            if (!proxy) {
                socket.bind(address); // Standard publisher
            } else {
                socket.connect(address); // Publish to broker
            }
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
