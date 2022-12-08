#include <iostream>
#include "tools/encryption_keygen_core.h"

int main()
{
    std::string publicKey, secretKey;
    au::commtool::EncryptionKeygen encryptionKeygen;
    if (encryptionKeygen.Generate(publicKey, secretKey)) {
        std::cout << "public key: " << publicKey << std::endl;
        std::cout << "secret key: " << secretKey << std::endl;
    } else {
        std::cout << "generated key pair failed!" << std::endl;
    }
    return 0;
}
