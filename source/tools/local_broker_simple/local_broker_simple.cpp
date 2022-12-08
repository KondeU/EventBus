#include <csignal>
#include <thread>
#include <string>
#include <unordered_map>
#include <iostream>
#include "tools/local_broker_core.h"

bool g_exit = false;
std::unordered_map<std::string, std::string> g_configs;

void SignalHandler(int signum)
{
    if (signum == SIGINT) {
        g_exit = true;
    }
}

void Configure(std::string config)
{
    // config string pattern: "key=value"
    size_t split = config.find_first_of('=');
    std::string key = config.substr(0, split);
    std::string value = config.substr(split + 1);
    g_configs[key] = value;
}

void Configure(int argc, char* argv[])
{
    for (int argn = 1; argn < argc; argn++) {
        Configure(argv[argn]);
    }
}

int main(int argc, char* argv[])
{
    std::cout << "Local Broker running..." << std::endl;
    signal(SIGINT, SignalHandler);
    Configure(argc, argv);
    int port1 = 6081, port2 = 6082;
    try { port1 = std::stoi(g_configs["p1"]); } catch (...) {}
    try { port2 = std::stoi(g_configs["p2"]); } catch (...) {}
    std::cout << "port1 is: " << port1 << std::endl;
    std::cout << "port2 is: " << port2 << std::endl;
    au::commtool::LocalBroker localBroker;
    localBroker.Start(port1, port2);
    while (!g_exit) {
        std::this_thread::sleep_for( // 20Hz
            std::chrono::milliseconds(50));
    }
    localBroker.Stop();
    return 0;
}
