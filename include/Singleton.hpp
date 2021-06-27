#pragma once

template <typename T, int N = 0>
class Singleton {
public:
    static T& GetRef()
    {
        static T instance{};
        return instance;
    }

    T& operator()() const
    {
        return reference;
    }

private:
    static T& reference;
};

template <typename T, int N>
T& Singleton<T, N>::reference = Singleton<T, N>::GetRef();
