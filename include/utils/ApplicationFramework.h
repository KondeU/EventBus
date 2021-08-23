#pragma once

#include "../TiBus.hpp"

class ApplicationFramework {
public:
    ApplicationFramework()
    {
    }

    void Configure(std::string config)
    {
    }

    void Configure(int argc, char* argv[])
    {
    }

    void Run()
    {
        OnPrepare();
        // TODO
        OnBeforeLoop();
        while (loop) {
            // TODO
        }
        OnAfterLoop();
        // TODO
        OnExit();
    }

    void Exit()
    {
        loop = false;
    }

    virtual void OnPrepare() {}
    virtual void OnExit() {}

    virtual void OnBeforeLoop() {}
    virtual void OnAfterLoop() {}

    virtual void OnBeforeTick() {}
    virtual void OnAfterTick() {}

protected:
    bool loop = true;
    std::unordered_map<std::string, std::string> configs;
};
