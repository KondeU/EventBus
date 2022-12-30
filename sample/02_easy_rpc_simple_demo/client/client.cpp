#include <csignal>
#include "EventBus.hpp"

bool g_loop = true;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_loop = false;
    }
}

void Proc_TestBroadcast(unsigned int value) // It will be called in broadcast's sub proc thread.
{
    std::cout << "Proc_TestBroadcast Callee! value is " << value << std::endl;
}

int main(int argc, char* argv[])
{
    au::rpc::RpcStandardRequest request;
    request.ConnectNetwork("tcp://127.0.0.1", 6088);

    au::rpc::RpcAsyncBroadcast broadcast;
    broadcast.BindFunc("Proc_TestBroadcast", Proc_TestBroadcast); // !!! Bind Function !!!
    broadcast.Start(au::rpc::RpcAsyncBroadcast::Role::Client, "tcp://127.0.0.1", 6098, 6099);

    for (unsigned int count = 1; g_loop; count++) {
        auto ret1 = request.CallFunc<long>("Proc_TestRequest1", count); // !!! Call Function !!!
        std::cout << "CallFunc: long Proc_TestRequest1(unsigned int), "
            << "error value is " << int(ret1.error) << ", "
            << "return value is " << ret1.value << std::endl;
        auto ret2 = request.CallFunc<void>("Proc_TestRequest2", count); // !!! Call Function !!!
        std::cout << "CallFunc: Proc_TestRequest2(), "
            << "error value is " << int(ret2.error) << std::endl;

        broadcast.CallFunc("Proc_TestBroadcast", count); // !!! Call Function !!!
        std::cout << "CallFunc: void Proc_TestBroadcast(unsigned int)" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    request.DisconnectNetwork();
    broadcast.Stop();

    return 0;
}
