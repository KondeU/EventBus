#pragma once

#include "TickBus.hpp"

class ApplicationFramework {
public:
    ApplicationFramework()
    {
        // Default hosts broker configuration
        configs["hbip"] = "127.0.0.1"; // Default hosts broker ip

        // Default hosts broker configuration
        configs["hbp1"] = "6083"; // Default hosts broker port 1 (Sub H_B)
        configs["hbp2"] = "6084"; // Default hosts broker port 2 (Pub H_B)

        // Default local broker configuration
        configs["lbp1"] = "6081"; // Default local broker port 1 (Sub L_B)
        configs["lbp2"] = "6082"; // Default local broker port 2 (Pub L_B)

        // Default level configuration, Only could be: I_P(inproc broker),
        configs["level"] = "I_P"; // H_B (hosts broker), L_B(local broker)
    }

    void Configure(std::string config)
    {
        // config string pattern: "key=value"
        size_t split = config.find_first_of('=');
        std::string key = config.substr(0, split);
        std::string value = config.substr(split + 1);
        configs[key] = value;
    }

    void Configure(int argc, char* argv[])
    {
        for (int argn = 1; argn < argc; argn++) {
            Configure(argv[argn]);
        }
    }

    int GetConfigInt(const std::string& item)
    {
        return atoi(configs[item].c_str());
    }

    long GetConfigLong(const std::string& item)
    {
        return atol(configs[item].c_str());
    }

    long long GetConfigLongLong(const std::string& item)
    {
        return atoll(configs[item].c_str());
    }

    double GetConfigDouble(const std::string& item)
    {
        return atof(configs[item].c_str());
    }

    std::string GetConfig(const std::string& item)
    {
        return configs[item];
    }

    virtual bool Run()
    {
        loop = true;

        if (!OnBeginRun()) {
            OnExit();
            loop = false;
            return false;
        }

        tibus::BusManager::BusGroupLevel level =
            tibus::BusManager::BusGroupLevel::InProcess;
        std::string levelConfig = GetConfig("level");
        if (levelConfig == "H_B") {
            level = tibus::BusManager::BusGroupLevel::MultiHost;
        } else if (levelConfig == "L_B") {
            level = tibus::BusManager::BusGroupLevel::LocalHost;
        }

        tibus::BusManager::BusInfo info;
        info.h.emplace_back(
            tibus::BusManager::BusInfo::HostsBrokerInfo {
            GetConfig("hbip"),    // --> info.h[0].ip = GetConfig("hbip")
            GetConfigInt("hbp1"), // --> info.h[0].sport = GetConfigInt("hbp1")
            GetConfigInt("hbp2")  // --> info.h[0].pport = GetConfigInt("hbp2")
        });
        info.l.sport = GetConfigInt("lbp1");
        info.l.pport = GetConfigInt("lbp2");

        if (!tibus::BusManager::GetReference().Start(level, info)) {
            OnExit();
            loop = false;
            return false;
        }

        if (!OnBeforeLoop()) {
            OnExit();
            loop = false;
            return false;
        }

        while (loop) {
            if (!OnBeforeTick()) {
                OnExit();
                loop = false;
                return false;
            }

            TickBus::GetReference().EventNow(&TickBusEvent::OnTick);

            if (!OnAfterTick()) {
                OnExit();
                loop = false;
                return false;
            }

            tibus::BusManager::GetReference().ExecuteReceivedEvents();
        }

        if (!OnAfterLoop()) {
            OnExit();
            return false;
        }

        if (!tibus::BusManager::GetReference().Stop()) {
            OnExit();
            return false;
        }

        if (!OnEndRun()) {
            OnExit();
            return false;
        }

        OnExit();
        return true;
    }

    virtual void Exit()
    {
        loop = false;
    }

protected:
    virtual bool OnBeginRun()   { return true; }
    virtual bool OnEndRun()     { return true; }

    virtual bool OnBeforeLoop() { return true; }
    virtual bool OnAfterLoop()  { return true; }

    virtual bool OnBeforeTick() { return true; }
    virtual bool OnAfterTick()  { return true; }

    virtual bool OnExit()       { return true; }

private:
    bool loop = false;
    std::unordered_map<std::string, std::string> configs;
};
