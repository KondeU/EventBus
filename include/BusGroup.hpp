#pragma once

namespace tibus {

class BusGroup {
public:
    virtual void Update() = 0;
};

class BusGroupEx : public BusGroup {
public:
    void BindBus(const std::string& name, const BusTraitBase& bus)
    {
    }

    template <class ...Args>
    void BindFunction(const std::string& name, std::function<void(Args...)> func)
    {
    }

    template <class ...Args>
    void BindFunction(const BusTraitBase& bus, std::function<void(Args...)> func)
    {
    }

protected:
    template <class ...Args>
    void FuncCallWrapper(std::function<void(Args...)> func, const std::string& data)
    {
    }

    template <typename ArgsTuple>
    inline void ExtractArgs(const std::string& data, ArgsTuple& extract)
    {
    }

    template <typename ArgsTuple, std::size_t ...Index>
    inline void ExtractArgsImpl(const std::string& data, ArgsTuple& extract,
        std::index_sequence<Index...>) // `Index` used for extracting params pack
    {
    }

private:
    // HMAP<BusName, PAIR<BusTraitBasePtr, HMAP<FunctionName, FunctionWrapper>>>
    std::unordered_map<std::string, std::pair<BusTraitBase*, std::unordered_map<
        std::string, std::function<void(const std::string&)>>>> functions;
    std::unordered_map<BusTraitBase*, std::string> buses;

    serialize::Serializer sin;
    serialize::Serializer sout;

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

class InProcessBusGroup : public BusGroup
    , public Singleton<InProcessBusGroup> {
public:
    void Update() override
    {
    }

    template <class ...Args>
    inline void BindFunction(const std::string& name, std::function<void(Args...)> func) {}

    template <class ...Args>
    inline void BindFunction(const BusTraitBase& bus, std::function<void(Args...)> func) {}
};

class LocalHostBusGroup : public BusGroupEx
    , public Singleton<LocalHostBusGroup> {
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

class MultiHostBusGroup : public BusGroupEx
    , public Singleton<MultiHostBusGroup> {
public:
    bool Start(const std::string& brokerIp, int subPort, int pubPort)
    {
    }

    bool Stop()
    {
    }

private:
    int portOfSub, portOfPub;
    const std::string ipOfHostsBroker;
};

}
