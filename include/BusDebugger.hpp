#pragma once

#include <thread>
#include <cassert>
#include "GlobalSingleton.hpp"

namespace au {
namespace ebus {

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
}
