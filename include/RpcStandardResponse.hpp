#pragma once

#include "Serializer.hpp"
#include "CommunicateContext.hpp"
#include "RpcBasicTemplate.hpp"

namespace au {
namespace rpc {

class RpcStandardResponse {
public:
    RpcStandardResponse()
    {
        process = // NB: Caution that the life cycle of process!
        [this](const std::string& request, std::string& respond) {
            ExecFunc(request, respond);
        };
    }

    bool StartProcess(const std::string& selfIp, int selfPort)
    {
        if (responder != nullptr) {
            return false;
        }

        responder = communicate::CommunicateContext::GetReference()
            .Create<communicate::Responder>(selfIp + ":" + std::to_string(selfPort));
        if (responder == nullptr) {
            return false;
        }

        if (!responder->StartResponse(process)) {
            return false;
        }

        return true;
    }

    bool StopProcess()
    {
        if (responder == nullptr) {
            return false;
        }

        if (!responder->StopResponse() ||
            !responder->WaitResponse() ||
            !responder->ResetResponse()) {
            return false;
        }

        communicate::CommunicateContext::GetReference().Destroy(responder);

        responder = nullptr;
        return true;
    }

    template <typename Func>
    inline void BindFunc(const std::string& name, Func func)
    {
        rpcs[name] = std::bind(&RpcProcessResponse::CallProxy<Func>, this,
            func, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename CFunc, typename CType>
    inline void BindFunc(const std::string& name, CFunc func, CType& impl)
    {
        rpcs[name] = std::bind(&RpcProcessResponse::CallProxy<CFunc, CType>, this,
            func, &impl, std::placeholders::_1, std::placeholders::_2);
    }

protected:
    void ExecFunc(const std::string& request, std::string& respond)
    {
        std::string funcName;
        serializer.Deserialize(request, funcName);

        auto funcIter = rpcs.find(funcName);
        if (funcIter != rpcs.end()) {
            funcIter->second(request, respond);
        } else {
            rpc::RpcReturnWrapper<void> funcRetv;
            std::get<rpc::RpcReturnCode>(funcRetv) =
                rpc::RpcReturnCode::FunctionNotFound;
            serializer.Serialize(respond, funcName, funcRetv);
        }
    }

    template <typename Func>
    inline void CallProxy(Func func,
        const std::string& request, std::string& respond)
    {
        CallProxyImpl(func, request, respond);
    }

    template <typename CFunc, typename CType>
    inline void CallProxy(CFunc func, CType* impl,
        const std::string& request, std::string& respond)
    {
        CallProxyImpl(func, impl, request, respond);
    }

    template <typename R, class ...Params>
    void CallProxyImpl(R(*func)(Params...),
        const std::string& request, std::string& respond)
    {
        CallProxyImpl(std::function<R(Params...)>(func), request, respond);
    }

    template <typename R, typename C, class ...Params>
    void CallProxyImpl(R(C::*func)(Params...), C* impl,
        const std::string& request, std::string& respond)
    {
        auto proxy = [&func, &impl](Params ...params)->R {
            return (impl->*func)(params...);
        };
        CallProxyImpl(std::function<R(Params...)>(proxy), request, respond);
    }

    template <typename R, class ...Params>
    void CallProxyImpl(std::function<R(Params...)> func,
        const std::string& request, std::string& respond)
    {
        std::string funcName;
        rpc::RpcFuncArgsWrapper<typename std::decay<Params>::type...> funcArgs;
        serializer.Deserialize(request, funcName, funcArgs);

        typename rpc::RpcReturnType<R>::Type retv = CallInterface<R>(func, funcArgs);

        rpc::RpcReturnWrapper<R> funcRetv;
        std::get<rpc::RpcReturnCode>(funcRetv) = rpc::RpcReturnCode::Success;
        std::get<typename rpc::RpcReturnType<R>::Type>(funcRetv) = retv;
        serializer.Serialize(respond, funcName, funcRetv);
    }

    template <typename Retv, typename Func, typename ArgsTuple>
    typename std::enable_if<!std::is_same<Retv, void>::value,
    typename rpc::RpcReturnType<Retv>::Type>::type
    CallInterface(Func func, ArgsTuple args)
    {
        return CallInvoke(func, args);
    }

    template <typename Retv, typename Func, typename ArgsTuple>
    typename std::enable_if<std::is_same<Retv, void>::value,
    typename rpc::RpcReturnType<Retv>::Type>::type
    CallInterface(Func func, ArgsTuple args)
    {
        CallInvoke(func, args);
        return uint8_t(0);
    }

    template <typename Func, typename ArgsTuple>
    decltype(auto) CallInvoke(Func&& func, ArgsTuple&& argsTuple)
    {
        constexpr auto Size = std::tuple_size<
            typename std::decay<ArgsTuple>::type>::value;
        return CallInvokeImpl(std::forward<Func>(func),
            std::forward<ArgsTuple>(argsTuple),
            std::make_index_sequence<Size>{});
    }

    template <typename Func, typename ArgsTuple, std::size_t ...Index>
    decltype(auto) CallInvokeImpl(Func&& func, ArgsTuple&& argsTuple,
        std::index_sequence<Index...>)
    {
        (void)argsTuple; // Avoid the compilation warning
        // that `the variable has been defined but not used`
        // if the number of parameters of the function is 0.
        return func(std::get<Index>(std::forward<ArgsTuple>(argsTuple))...);
    }

private:
    std::unordered_map<std::string, std::function<
        void(const std::string&, std::string&)>> rpcs;
    std::function<void(const std::string&, std::string&)> process;

    serialize::NetBinSerializer serializer;
    communicate::Responder* responder = nullptr;
};

}
}
