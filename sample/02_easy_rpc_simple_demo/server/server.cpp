#include <csignal>
#include "EventBus.hpp"

bool g_loop = true;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_loop = false;
    }
}

long Proc_TestRequest(unsigned int value)
{
    std::cout << "Proc_TestRequest1 Callee! value is " << value << std::endl;
    return value;
}

class Proc_TestRequest_Class {
public:
    long Proc_TestRequest(unsigned int value)
    {
        std::cout << "Proc_TestRequest2 Callee! value is " << value << std::endl;
        return value;
    }
} g_Proc_TestRequest_Class;

void Proc_TestBroadcast(unsigned int value)
{
    // It will be called in broadcast's sub proc thread, and if broadcast role is server,
    // it will broadcast this call to all clients.
    std::cout << "Proc_TestBroadcast Callee! value is " << value << std::endl;
}

int main(int argc, char* argv[])
{
    au::rpc::RpcStandardResponse response;
    response.BindFunc("Proc_TestRequest1", Proc_TestRequest); // !!! Bind Function !!!
    response.BindFunc("Proc_TestRequest2",                     // !!! Bind Function !!!
        &Proc_TestRequest_Class::Proc_TestRequest, g_Proc_TestRequest_Class);
    response.StartProcess("tcp://0.0.0.0", 6088);

    au::rpc::RpcAsyncBroadcast broadcast;
    broadcast.BindFunc("Proc_TestBroadcast", Proc_TestBroadcast); // !!! Bind Function !!!
    broadcast.Start(au::rpc::RpcAsyncBroadcast::Role::Server,
        "tcp://127.0.0.1", // DO NOT USE 0.0.0.0! Because it is both a server and a client!
        6098, 6099);

    for (unsigned int count = 1; g_loop; count++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    response.StopProcess();
    broadcast.Stop();

    return 0;
}
