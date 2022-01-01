#include <iostream>
#include "zmq.h"

int main()
{
    char pk[41] = ""; // public key
    char sk[41] = ""; // secret key
    int rc = zmq_curve_keypair(pk, sk);
    if (rc == 0) {
        std::cout << "public key: " << pk << std::endl;
        std::cout << "secret key: " << sk << std::endl;
    } else {
        std::cout << "generated key pair failed, "
                     "return value is " << rc << std::endl;
    }
    system("pause");
    return 0;
}
