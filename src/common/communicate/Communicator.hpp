#pragma once

#include "Singleton.hpp"
#include "Requester.h"
#include "Responder.h"
#include "Subscriber.h"
#include "Publisher.h"

namespace ti {
namespace communicate {

class Communicator {
public:
    using InstValue = uintptr_t;

    template <typename Inst>
    inline InstValue MakeInstValue(Inst inst) const
    {
        CheckInst<Inst>();
        return reinterpret_cast<InstValue>(inst);
    }

    template <typename Inst>
    inline void ResetInstInvalid(Inst& inst)
    {
        CheckInst<Inst>();
        inst = nullptr;
    }

    template <typename Inst>
    inline bool IsInstInvalid(Inst inst) const
    {
        CheckInst<Inst>();
        return (inst == nullptr);
    }

    RequesterInst CreateRequester(const std::string& toServer)
    {
        return CreateInstance(requesters, toServer);
    }

    ResponderInst CreateResponder(const std::string& server)
    {
        return CreateInstance(responders, server);
    }

    SubscriberInst CreateSubscriber(const std::string& pubServer)
    {
        return CreateInstance(subscribers, pubServer);
    }

    PublisherInst CreatePublisher(const std::string& server)
    {
        return CreateInstance(publishers, server);
    }

    bool DestroyInstance(InstValue instValue)
    {
        if (requesters.erase(instValue) > 0) {
            return true;
        }
        if (responders.erase(instValue) > 0) {
            return true;
        }

        if (subscribers.erase(instValue) > 0) {
            return true;
        }
        if (publishers.erase(instValue) > 0) {
            return true;
        }

        return false;
    }

    virtual ~Communicator()
    {
        context().shutdown();
    }

protected:
    template <typename T>
    using Container = std::unordered_map<InstValue, std::unique_ptr<T>>;

    template <typename T, class ...Args>
    inline auto CreateInstance(Container<T>& container, const Args& ...args)
    {
        auto instance = new T(context());
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
        container.emplace(MakeInstValue(instance), instance);
        return instance;
    }

    template <typename Inst>
    struct CheckInst {
        static_assert(
            std::is_same< Inst, RequesterInst  >::value ||
            std::is_same< Inst, ResponderInst  >::value ||
            std::is_same< Inst, SubscriberInst >::value ||
            std::is_same< Inst, PublisherInst  >::value,
            "Instance invalid.");
    };

private:
    Singleton<zmq::context_t> context;

    Container<Requester> requesters;
    Container<Responder> responders;

    Container<Subscriber> subscribers;
    Container<Publisher> publishers;
};

}
}
