#pragma once

#include "NonCopyable.hpp"

namespace tibus {
namespace common {

template <typename T>
class Singleton : public NonCopyable {
public:
    static T& GetReference()
    {
        return instance;
    }

    ~Singleton() = default;

protected:
    Singleton() = default;

private:
    static T instance;
};

template <typename T>
T Singleton<T>::instance;

}
}
