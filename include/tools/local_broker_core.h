#pragma once

namespace au {
namespace communicate {

class Broker;

}
}

namespace au {
namespace commtool {

class LocalBroker {
public:
    bool Start(int port1, int port2);
    void Stop();

    virtual ~LocalBroker();

private:
    communicate::Broker* broker = nullptr;
};

}
}
