#include "tools/encryption_keygen_core.h"
#include "CommunicateContext.hpp"

namespace au {
namespace commtool {

bool EncryptionKeygen::Generate(std::string& publicKey, std::string& secretKey)
{
    char szPublicKey[41] = "";
    char szSecretKey[41] = "";
    int rc = zmq_curve_keypair(szPublicKey, szSecretKey);
    publicKey = szPublicKey;
    secretKey = szSecretKey;
    return (rc == 0);
}

}
}
