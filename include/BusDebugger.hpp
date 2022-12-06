#pragma once

#include <thread>
#include <cassert>
#include "common/GlobalSingleton.hpp"

namespace au {

class BusDebugger : public common::GlobalSingleton<BusDebugger> {
public:
    void ThreadCheckAssert()
    {
        assert(ThreadCheck());
    }

    bool ThreadCheck()
    {
        if (tid == std::thread::id{}) {
            tid = std::this_thread::get_id();
        } else {
            return (tid == std::this_thread::get_id());
        }
        return true;
    }

private:
    std::thread::id tid;
};

}
