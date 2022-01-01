#pragma once

#include <mutex>
#include "common/Any.hpp"
#include "common/Binder.hpp"
#include "serialize/Serializer.hpp"
#include "communicate/CommunicateContext.hpp"
#include "BusTraitBase.hpp"

namespace tibus {

class BusGroup {
public:
    virtual void Update() = 0;
};

class BusGroupEx : public BusGroup {
public:
    // Use this type to determine the serialization processor: It
    // could be CerealSerializeProcesser, MsgpackSerializeProcesser
    // or custom serialization processor.
    using SerializeProcesser = serialize::CerealSerializeProcesser;

    void BindBus(const std::string& name, const BusTraitBase& bus)
    {
        buses[&bus] = name;
        functions[name].first = &bus;
        if (subscriber) {
            subscriber->Subscribe(name);
        }
    }

    template <class ...Args>
    void BindFunction(const std::string& name, const BusTraitBase& bus,
        std::function<void(std::string, Args...)> function) // proxy function
    {
        functions[buses[&bus]].second()[name] = std::bind(
            &BusGroupEx::FuncCallWrapper<Args...>, this,
            std::placeholders::_1, function);
    }

    template <class ...Args>
    void CallFunction(const BusTraitBase& bus, const std::string& function,
        const std::string& handler, const Args& ...args)
    {
        // CallFunction in BusGroupEx is a remote function call, and the remote
        // Bus cannot be found if only the reference of BusTraitBase is provided.
    }

    template <class ...Args>
    void CallFunction(const std::string& bus, const std::string& function,
        const std::string& handler, const Args& ...args)
    {
        if (!publisher) {
            return;
        }
        std::vector<std::string> envelopeAndContentsPack;
        envelopeAndContentsPack.resize(3);
        envelopeAndContentsPack[0] = bus;
        envelopeAndContentsPack[1] = function;
        sin.Serialize(envelopeAndContentsPack[2], handler, args...);
        publisher->Publish(envelopeAndContentsPack);
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(const std::string& data,
        std::function<void(std::string, Args...)> func)
    {
        std::tuple<std::string, Args...> args;
        ExtractArgs(data, args); // args: handler name(id) + function real arguments
        // Deserialize and extract the parameter package in the background receiving
        // thread and then package the function and parameters into the `actions`.
        FuncCallWrapperImpl(func, args, std::make_index_sequence<1 + sizeof...(Args)>{});
    }

    template <typename Func, typename Args, std::size_t ...Index>
    void FuncCallWrapperImpl(const Func& func, const Args& args,
        std::index_sequence<Index...>) // `Index` used for extracting args pack
    {
        std::lock_guard<std::mutex> locker(mutex);
        actions.emplace_back(std::bind(func, std::get<Index>(args)...));
    }

    template <typename ArgsTuple>
    inline void ExtractArgs(const std::string& data, ArgsTuple& extract)
    {
        ExtractArgsImpl(data, extract, std::make_index_sequence<
            std::tuple_size<typename std::decay<ArgsTuple>::type>::value>{});
    }

    template <typename ArgsTuple, std::size_t ...Index>
    inline void ExtractArgsImpl(const std::string& data, ArgsTuple& extract,
        std::index_sequence<Index...>) // `Index` used for extracting params pack
    {
        sout.Deserialize(data, std::get<Index>(extract)...);
    }

private:
    // HMAP<BusName, PAIR<BusTraitBaseCPtr, HMAP<FunctionName, FunctionWrapper>>>
    //std::unordered_map<std::string, std::pair<const BusTraitBase*, std::unordered_map<   --- original source
    //    std::string, std::function<void(const std::string&)>>>> functions;               --- original source
    // Avoid the compile warning C4503 when using the MSVC Compiler (VS2015):
    // - The decorated name was longer than the compiler limit (4096), and was truncated.
    // - To avoid this warning and the truncation, reduce the number of arguments or name
    // - length of identifiers used.
    // see: https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2008/074af4b6(v=vs.90)?redirectedfrom=MSDN
    struct FunctionHashmap {
        using Type = std::unordered_map<std::string,
            std::function<void(const std::string&)>>;

        operator Type& ()
        {
            return inBusFunctions;
        }

        Type& operator()()
        {
            return inBusFunctions;
        }

        Type inBusFunctions;
    };

    std::unordered_map<std::string, std::pair<const BusTraitBase*, FunctionHashmap>> functions;
    std::unordered_map<const BusTraitBase*, std::string> buses;

    serialize::Serializer<SerializeProcesser> sin;
    serialize::Serializer<SerializeProcesser> sout;

    std::vector<std::function<void()>> actions;
    std::mutex mutex; // actions' mutex

public:
    void Update() override
    {
        std::vector<std::function<void()>> executes;
        {
            std::unique_lock<std::mutex> locker(mutex, std::try_to_lock);
            if (locker.owns_lock()) {
                executes.swap(actions);
            }
        }
        for (const auto& execute : executes) {
            execute();
        }
    }

protected:
    void Process(const std::string& envelope, const std::vector<std::string>& contents)
    {
        if (contents.size() != 2) {
            return; // If run here, it may be an internal error or an external attack!
        }
        const std::string& busName = envelope;
        const std::string& funcName = contents[0];
        const std::string& idAndArgs = contents[1];
        // Searching layer by layer and find the executor for execution.
        const auto& busIter = functions.find(busName);
        if (busIter == functions.end()) {
            return;
        }
        const auto& funcIter = busIter->second.second().find(funcName);
        if (funcIter == busIter->second.second().end()) {
            return;
        }
        funcIter->second(idAndArgs);
    }

    void Timeout(bool isTimeout) // FIXME: subscriber receive timeout
    {
    }

    // I have wondered for a long time whether subscriber and publisher should be in the
    // LocalHostBusGroup and MultiHostBusGroup, because BusGroupEx was originally designed
    // as a layer with serialization and logic processing. The data transmission is expected
    // to be placed in the LocalHostBusGroup and MultiHostBusGroup.
    // But I decided to place them here finally, because it can simplify processing and reduce
    // repeated code. And I think we can move subscriber and publisher to the subclasses
    // if the logic of the LocalHostBusGroup and MultiHostBusGroup change in the future.
    communicate::Subscriber* subscriber = nullptr;
    communicate::Publisher* publisher = nullptr;

    bool StartCommunication(const std::string& subAddr, const std::string& pubAddr)
    {
        if (!subscriber) {
            subscriber = communicate::CommunicateContext::GetReference()
                .Create<communicate::Subscriber>(subAddr);
            if (subscriber) {
                for (const auto& bus : buses) {
                    subscriber->Subscribe(bus.second);
                }
                subscriber->StartReceive(
                    common::Bind(&BusGroupEx::Timeout, this),
                    common::Bind(&BusGroupEx::Process, this));
            }
        }
        if (!publisher) {
            publisher = communicate::CommunicateContext::GetReference()
                .Create<communicate::Publisher>(pubAddr, true);
        }
        return (subscriber && publisher);
    }

    bool StopCommunication()
    {
        if (subscriber) {
            subscriber->StopReceive();
            subscriber->WaitReceive();
            subscriber->ResetReceive();
            if (communicate::CommunicateContext::GetReference()
                .Destroy(subscriber)) {
                subscriber = nullptr;
            }
        }
        if (publisher) {
            if (communicate::CommunicateContext::GetReference()
                .Destroy(publisher)) {
                publisher = nullptr;
            }
        }
        return ((!subscriber) && (!publisher));
    }
};

class InProcessBusGroup : public BusGroup, public common::GlobalSingleton<InProcessBusGroup> {
public:
    void BindBus(const std::string& name, const BusTraitBase& bus)
    {
        buses[&bus] = name;
        functions[name].first = &bus;
    }

    template <class ...Args>
    void BindFunction(const std::string& name, const BusTraitBase& bus,
        std::function<void(std::string, Args...)> function) // proxy function
    {
        functions[buses[&bus]].second()[name] = std::bind(
            &InProcessBusGroup::FuncCallWrapper<Args...>,
            this, std::placeholders::_1, function);
    }

    template <class ...Args>
    void CallFunction(const BusTraitBase& bus, const std::string& function,
        const std::string& handler, const Args& ...args)
    {
        const auto& iter = buses.find(&bus);
        if (iter == buses.end()) {
            return;
        }
        CallFunction(iter->second, function, handler, args...);
    }

    template <class ...Args>
    void CallFunction(const std::string& bus, const std::string& function,
        const std::string& handler, const Args& ...args)
    {
        const auto& busIter = functions.find(bus);
        if (busIter == functions.end()) {
            return;
        }
        const auto& funcIter = busIter->second.second().find(function);
        if (funcIter == busIter->second.second().end()) {
            return;
        }
        funcIter->second(std::tuple<std::string, Args...> { handler, args... });
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(const common::Any& pack,
        std::function<void(std::string, Args...)> func)
    {
        // pack: handler name(id) + function real arguments
        using Pack = std::tuple<std::string, Args...>;
        if (pack.IsNull() || !pack.IsType<Pack>()) {
            return;
        }
        FuncCallWrapperImpl(func, pack.AnyCast<Pack>(),
            std::make_index_sequence<1 + sizeof...(Args)>{});
    }

    template <typename Func, typename ArgsTuple, std::size_t ...Index>
    void FuncCallWrapperImpl(const Func& func, const ArgsTuple& args,
        std::index_sequence<Index...>)
    {
        executes.emplace_back(std::bind(func, std::get<Index>(args)...));
    }

public:
    void Update() override
    {
        for (const auto& execute : executes) {
            execute();
        }
        executes.clear();
    }

private:
    // PLS see BusGroupEx::FunctionHashmap
    struct FunctionHashmap {
        using Type = std::unordered_map<std::string,
            std::function<void(const common::Any&)>>;

        operator Type& ()
        {
            return inBusFunctions;
        }

        Type& operator()()
        {
            return inBusFunctions;
        }

        Type inBusFunctions;
    };

    std::unordered_map<std::string, std::pair<const BusTraitBase*, FunctionHashmap>> functions;
    std::unordered_map<const BusTraitBase*, std::string> buses;

    std::vector<std::function<void()>> executes;
};

class LocalHostBusGroup : public BusGroupEx, public common::GlobalSingleton<LocalHostBusGroup> {
public:
    bool Start(int subPort, int pubPort)
    {
        portOfSub = subPort;
        portOfPub = pubPort;
        std::string subAddr = "tcp://" + ipOfLocalBroker + ":" + std::to_string(portOfSub);
        std::string pubAddr = "tcp://" + ipOfLocalBroker + ":" + std::to_string(portOfPub);
        return StartCommunication(subAddr, pubAddr);
    }

    bool Stop()
    {
        return StopCommunication();
    }

private:
    int portOfSub = 0, portOfPub = 0;
    const std::string ipOfLocalBroker = "127.0.0.1";
};

class MultiHostBusGroup : public BusGroupEx, public common::GlobalSingleton<MultiHostBusGroup> {
public:
    bool Start(const std::string& brokerIp, int subPort, int pubPort)
    {
        portOfSub = subPort;
        portOfPub = pubPort;
        ipOfHostsBroker = brokerIp;
        std::string subAddr = "tcp://" + ipOfHostsBroker + ":" + std::to_string(portOfSub);
        std::string pubAddr = "tcp://" + ipOfHostsBroker + ":" + std::to_string(portOfPub);
        return StartCommunication(subAddr, pubAddr);
    }

    bool Stop()
    {
        return StopCommunication();
    }

private:
    int portOfSub = 0, portOfPub = 0;
    std::string ipOfHostsBroker;
};

}
