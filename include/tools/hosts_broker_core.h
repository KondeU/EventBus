#pragma once

namespace au {
namespace communicate {

class Broker;

}
}

namespace au {
namespace commtool {

class HostsBroker {
public:
    bool Start(int port1, int port2);
    void Stop();

    virtual ~HostsBroker();

private:
    communicate::Broker* broker = nullptr;
};

}
}
