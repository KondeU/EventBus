#pragma once

#include <functional>

namespace tibus {
namespace common {

class ActionExecutor {
public:
    virtual void Execute() = 0;
};

template <class ...Args>
class Action : public ActionExecutor {
public:
    using ArgsTuple = std::tuple<Args...>;

    template <typename Func>
    Action(Func&& func, ArgsTuple&& args)
        : func(std::forward<Func>(func)), args(std::forward<ArgsTuple>(args))
    {
    }

    template <typename Func>
    explicit Action(Func&& func, Args&& ...args)
        : func(std::forward<Func>(func)), args(std::forward<Args>(args)...)
    {
    }

    void Execute() override
    {
        Invoke(func, args);
    }

protected:
    template <typename Func>
    inline void Invoke(Func& func, ArgsTuple& argsTuple)
    {
        constexpr auto Size = std::tuple_size<
            typename std::decay<ArgsTuple>::type>::value;
        InvokeImpl(func, argsTuple, std::make_index_sequence<Size>{});
    }

    template <typename Func, std::size_t ...Index>
    inline void InvokeImpl(Func& func, ArgsTuple& argsTuple,
        std::index_sequence<Index...>)
    {
        (void)argsTuple; // Avoid the compilation warning
        // that `the variable has been defined but not used`
        // if the number of parameters of the function is 0.
        func(std::get<Index>(argsTuple)...);
    }

private:
    std::function<void(Args...)> func;
    std::tuple<Args...> args; // ArgsTuple
};

template <typename Func, typename ...Args>
ActionExecutor* MakeAction(Func&& func, std::tuple<Args...>&& args)
{
    return new Action<Args...>(
        std::forward<Func>(func),
        std::forward<std::tuple<Args...>>(args));
}

template <typename Func, typename ...Args>
ActionExecutor* MakeAction(Func&& func, Args&& ...args)
{
    return new Action<Args...>(
        std::forward<Func>(func),
        std::forward<Args>(args)...);
}

}
}
