#pragma once

#include <mutex>
#include "serialize/Serializer.hpp"
#include "communicate/Communicator.hpp"
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
    }

    template <class ...Args>
    void BindFunction(const std::string& name,
        const BusTraitBase& bus, std::function<void(Args...)> func)
    {
        functions[buses[&bus]].second()[name] = std::bind(
            &BusGroupEx::FuncCallWrapper<Args...>, this,
            func, std::placeholders::_1);
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(std::function<void(Args...)> func, const std::string& data)
    {
        std::tuple<Args...> args;
        ExtractArgs(data, args);
        // Deserialize and extract the parameter package in the background receiving
        // thread and then package the function and parameters into the `actions`.
        FuncCallWrapperImpl(func, args, std::make_index_sequence<sizeof...(Args)>{});
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
        return ExtractArgsImpl(data, extract, std::make_index_sequence<
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
        operator std::unordered_map<std::string, std::function<void(const std::string&)>>& ()
        {
            return inBusFunctions;
        }
        std::unordered_map<std::string, std::function<void(const std::string&)>>& operator()()
        {
            return inBusFunctions;
        }
        std::unordered_map<std::string, std::function<void(const std::string&)>> inBusFunctions;
    };
    std::unordered_map<std::string, std::pair<const BusTraitBase*, FunctionHashmap>> functions;
    std::unordered_map<const BusTraitBase*, std::string> buses;

    serialize::Serializer<SerializeProcesser> sin;
    serialize::Serializer<SerializeProcesser> sout;

    communicate::Subscriber* subscriber = nullptr;
    communicate::Publisher* publisher = nullptr;

    std::vector<std::function<void()>> actions;
    std::mutex mutex; // actions' mutex

public:
    void Update() override
    {
    }

protected:
    void Process(const std::string& envelope, const std::string& content)
    {
    }
};

class InProcessBusGroup : public BusGroup, public common::GlobalSingleton<InProcessBusGroup> {
public:
    void Update() override
    {
    }

    template <class ...Args>
    inline void BindFunction(const std::string& name, std::function<void(Args...)> func) {}

    template <class ...Args>
    inline void BindFunction(const BusTraitBase& bus, std::function<void(Args...)> func) {}
};

class LocalHostBusGroup : public BusGroupEx, public common::GlobalSingleton<LocalHostBusGroup> {
public:
    bool Start(int subPort, int pubPort)
    {
    }

    bool Stop()
    {
    }

private:
    int portOfSub, portOfPub;
    const std::string ipOfLocalBroker = "127.0.0.1";
};

class MultiHostBusGroup : public BusGroupEx, public common::GlobalSingleton<MultiHostBusGroup> {
public:
    bool Start(const std::string& brokerIp, int subPort, int pubPort)
    {
    }

    bool Stop()
    {
    }

private:
    int portOfSub, portOfPub;
    std::string ipOfHostsBroker;
};

}
