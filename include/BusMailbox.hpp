#pragma once

#include <mutex>
#include "BusMailman.hpp"
#include "common/Action.hpp"
#include "serialize/Serializer.hpp"

namespace tibus {

enum class BusPolicy : int {
    Invalid = -1,
    InnerBus = 0, // In-process bus
    LocalBus = 1, // Local-host inter-process bus
    HostsBus = 2, // Cross-host inter-process bus
    MaxCount = 3
};

class BusMailbox : public BusMailman {
public:
    void BindTopic(const std::string& topicName,
        BusPolicy busPolicy = BusPolicy::InnerBus)
    {
        topic = topicName;
        policy = busPolicy;
    }

    template <class ...Args>
    void BindFunction(const std::string& name, std::function<void(Args...)> func)
    {
        functions[name] = std::bind(&BusMailbox::FuncCallWrapper<Args...>,
            this, func, std::placeholders::_1);
    }

    void Update()
    {
        std::vector<std::unique_ptr<common::ActionExecutor>> posts;
        if (mutex.try_lock()) {
            posts.swap(actions); // Swap in the lock zone.
            mutex.unlock();
        }
        for (auto& action : posts) {
            action->Execute();
        }
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(std::function<void(Args...)> func, const std::string& data)
    {
        std::tuple<typename std::decay<Args>::type...> args;
        ExtractArgs(data, args);
        // After here is in the lock zone.
        std::lock_guard<std::mutex> guard(mutex);
        actions.emplace_back(common::MakeAction(func, std::move(args)));
    }

    template <typename ArgsTuple>
    inline void ExtractArgs(const std::string& data, ArgsTuple& extract)
    {
        constexpr auto Size = std::tuple_size<
            typename std::decay<ArgsTuple>::type>::value;
        ExtractArgsImpl(data, extract, std::make_index_sequence<Size>{});
    }

    template <typename ArgsTuple, std::size_t ...Index>
    inline void ExtractArgsImpl(const std::string& data, ArgsTuple& extract,
        std::index_sequence<Index...>) // `Index` used for extracting params pack
    {
        (void)extract; // Avoid the compilation warning if params count is zero.
        serializer.Deserialize(data, std::get<Index>(extract)...);
    }

private:
    BusPolicy policy = BusPolicy::Invalid;

    std::string topic;
    std::unordered_map<std::string,
        std::function<void(const std::string&)>> functions;

    std::vector<std::unique_ptr<common::ActionExecutor>> actions;
    // The `actions` has a multithreading problem. It is executed and then
    // cleared in Update function, but added in the MailOffice thread.
    std::mutex mutex;

    #if defined SERIALIZE_FORCE_USE_CEREAL
    serialize::Serializer<serialize::CerealSerializeProcesser> serializer;
    #elif defined SERIALIZE_FORCE_USE_MSGPACK
    serialize::Serializer<serialize::MsgpackSerializeProcesser> serializer;
    #else
    serialize::Serializer<> serializer;
    #endif
};

}
