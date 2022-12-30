#pragma once

#include <zmq.hpp>

namespace au {
namespace communicate {

class Communicator {
protected:
    void SetupEncryption(zmq::socket_t& socket,
        const std::vector<std::string>& encryption)
    {
        if (encryption.size() == 1) { // ## curve server ##
            // Only need to provide the server-side secret key.
            SetupEncryption(socket, {}, encryption[0], {}, {}); // $PAIR: 1 secret
        } else
        if (encryption.size() == 3) { // ## curve client ##
            SetupEncryption(socket,
                encryption[0],  // server-side public key. $PAIR: 1 public
                {},             // blank reserved.
                encryption[1],  // client-side public key. $PAIR: 2 public
                encryption[2]); // client-side secret key. $PAIR: 2 secret
        }
    }

private:
    void SetupEncryption(zmq::socket_t& socket,
        std::string serverPublicKey, std::string serverSecretKey,
        std::string clientPublicKey, std::string clientSecretKey)
    {
        if (!serverSecretKey.empty()) { // ## curve server ##
            socket.set(zmq::sockopt::curve_server, true);
            socket.set(zmq::sockopt::curve_secretkey, serverSecretKey);
        } else {                        // ## curve client ##
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
