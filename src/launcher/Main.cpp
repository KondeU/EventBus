// FIXME: Temporary test used!

#include <csignal>
#include <iostream>
#include "serialize/Serializer.hpp"
//#include "communicate/Communicator.hpp"
using namespace tibus::serialize;
//using namespace tibus::communicate;

#include "BusMailbox.hpp"

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

    //auto pub = Communicator::GetReference().Create<Publisher>("tcp://127.0.0.1:6018");
    //Communicator::GetReference().Configure(2, 1000);
    //
    //int a, b;
    //double c;
    std::string data;
    //
    //Serializer<MsgpackSerializeProcesser> serializer;
    //serializer.Serialize(data, 1, 2, 1.2);
    //serializer.Deserialize(data, a, b, c);

    Serializer<MsgpackSerializeProcesser> serializer;
    serializer.Serialize(data);

    //tibus::BusMailbox mailbox;
    //mailbox.BindFunction("name", std::function<void()>([]() { std::cout << "test" << std::endl; }));
    //mailbox.Update();
    return 0;
}
