#pragma once

#include "common/GlobalSingleton.hpp"
#include "Requester.hpp"
#include "Responder.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "Broker.hpp"

namespace tibus {
namespace communicate {

class Communicator : public common::GlobalSingleton<Communicator> {
public:
    template <typename T, class ...Args>
    inline auto Create(const Args& ...args)
    {
        Container<T>& container = // See Container comment.
            common::Singleton<Container<T>>::GetReference();

        auto instance = new T(context);
        if (!instance) {
            // "!instance" means that
            // "instance == nullptr".
            return instance;
        }
        if (!instance->Init(args...)) {
            // Initialize failed.
            delete instance;
            instance = nullptr;
            return instance;
        }

        container.emplace(reinterpret_cast<uintptr_t>(instance), instance);
        return instance; // Return type: T*
    }

    template <typename T>
    bool Destroy(T* instance)
    {
        Container<T>& container = // See Container comment.
            common::Singleton<Container<T>>::GetReference();

        if (container.erase(instance) > 0) {
            return true;
        }
        return false;
    }

    void Configure(int ioThreadCount, int maxSocketCount)
    {
        // NB: The setting of the number of I/O threads must be set
        // before socket is created, otherwise it will not take effect.
        context.set(zmq::ctxopt::io_threads, ioThreadCount);
        context.set(zmq::ctxopt::max_sockets, maxSocketCount);
    }

    virtual ~Communicator()
    {
        context.shutdown();
    }

private:
    // Default setting of context:
    // I/O thread count: ZMQ_IO_THREADS_DFLT(1)
    // Max socket count: ZMQ_MAX_SOCKETS_DFLT(1023)
    // -- How to judge the number of IO threads:
    //    Generally, a thread can carry 1GBytes per second of traffic.
    // Communicator is a singleton class, so there is only one context.
    zmq::context_t context;

    // Communicator is already a singleton class, and the Container
    // inside Communicator is also written as a singleton so that
    // we can easily get the container when Create and Destroy.
    template <typename T>
    using Container = std::unordered_map<uintptr_t, std::unique_ptr<T>>;

    common::Singleton<Container<Requester>> requesters;
    common::Singleton<Container<Responder>> responders;

    common::Singleton<Container<Subscriber>> subscribers;
    common::Singleton<Container<Publisher>> publishers;

    common::Singleton<Container<Broker>> brokers;
};

}
}
