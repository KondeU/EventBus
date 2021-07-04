// FIXME: Temporary test used!

#include <csignal>
#include <iostream>
#include "serialize/Serializer.hpp"
using namespace tibus::serialize;

bool g_loop = true;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_loop = false;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);

    int a, b;
    double c;
    std::string data;

    Serializer<MsgpackSerializeProcesser> serializer;
    serializer.Serialize(data, 1, 2, 1.2);
    serializer.Deserialize(data, a, b, c);

    return 0;
}
