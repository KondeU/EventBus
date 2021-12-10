#pragma once

#include <functional>

namespace tibus {
namespace common {

#if _MSC_VER
template <int N>
using PlaceHolder = std::_Ph<N>;
#else
template <int N>
using PlaceHolder = std::_Placeholder<N>;
#endif

template <int N, int ...I>
struct Binder : Binder<N - 1, N - 1, I...> {};

template <int ...I>
struct Binder<1, I...> {
    template<typename C, typename R, class ...Args>
    static auto Bind(C* inst, R(C::* func)(Args...))
    {
        return std::bind(func, inst, PlaceHolder<I>{}...);
    }
};

template <typename C, typename R, class ...Args>
auto Bind(R(C::* func)(Args...), C* inst)
{
    return Binder<sizeof...(Args) + 1>::Bind(inst, func);
}

}
}
