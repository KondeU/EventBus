#pragma once

#include "BusMailman.hpp"
#include "common/Action.hpp"
#include "serialize/Serializer.hpp"

namespace tibus {

enum class BusPolicy {
    InnerBus = 0, // In-process bus
    LocalBus = 1, // Local-host inter-process bus
    HostsBus = 2, // Cross-host inter-process bus
    MaxCount = 3
};

class BusMailbox : public BusMailman {
public:
    void BindTopic(const std::string& topicName, BusPolicy busPolicy)
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
        for (auto& action : actions) {
            action->Execute();
        }
        actions.clear();
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(std::function<void(Args...)> func, const std::string& data)
    {
        std::tuple<typename std::decay<Args>::type...> args;
        serializer.Deserialize(data, std::get(args)...); // FIXME
    }

private:
    BusPolicy policy;

    std::string topic;
    std::unordered_map<std::string, std::function<
        void(const std::string& /*data*/)>> functions;

    std::vector<std::unique_ptr<common::ActionExecutor>> actions;

    serialize::Serializer<> serializer;
};

}
