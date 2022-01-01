#pragma once

#include <zmq.hpp>

namespace tibus {
namespace communicate {

class Communicator {
protected:
    void SetupEncryption(zmq::socket_t& socket,
        std::string serverPublicKey, std::string serverSecretKey,
        std::string clientPublicKey, std::string clientSecretKey)
    {
        if (!serverSecretKey.empty()) { // curve server
            socket.set(zmq::sockopt::curve_server, true);
            socket.set(zmq::sockopt::curve_secretkey, serverSecretKey);
        } else {
            if ((!serverPublicKey.empty()) &&
                (!clientPublicKey.empty()) &&
                (!clientSecretKey.empty())) {
                socket.set(zmq::sockopt::curve_server, false);
                socket.set(zmq::sockopt::curve_serverkey, serverPublicKey);
                socket.set(zmq::sockopt::curve_publickey, clientPublicKey);
                socket.set(zmq::sockopt::curve_secretkey, clientSecretKey);
            }
        }
    }
};

}
}
