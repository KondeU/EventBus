#pragma once

#include "common/Singleton.hpp"
#include "Requester.hpp"
#include "Responder.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "Broker.hpp"

namespace tibus {
namespace communicate {

class Communicator : public common::Singleton<Communicator> {
public:
    template <typename T, class ...Args>
    inline auto Create(const Args& ...args)
    {
        Container<T>& container =
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
        Container<T>& container =
            common::Singleton<Container<T>>::GetReference();

        if (container.erase(instance) > 0) {
            return true;
        }
        return false;
    }

    Communicator() : context(std::thread::hardware_concurrency() / 2)
    {
    }

    virtual ~Communicator()
    {
        context.shutdown();
    }

private:
    // Communicator is a singleton class,
    // so there is only one context.
    zmq::context_t context;

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
