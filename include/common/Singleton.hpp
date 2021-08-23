#pragma once

namespace tibus {
namespace common {

// Singleton base class, allow to use C++ CRTP.
template <typename T, int N = 0>
// Allows to specify the value of N to achieve
// a special scenario that there are different
// instances of the same type, breaking the
// singleton limit.
class Singleton {
public:
    static T& GetReference()
    {
        return instance;
    }

private:
    static T instance;
};

// If there is a class B that inherits class A,
// and class A inherits from Singleton<A>, and
// then an instance of B is created, then there
// will be two instances of A, one for Singleton<A>
// created in Singleton<A>::GetReference(), and
// one is in the B instance. So the singleton
// class is Singleton<A>, not A.
template <typename T, int N>
T Singleton<T, N>::instance;

}
}
