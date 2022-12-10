#pragma once

#include <cstdint>
#include <tuple>

namespace tirpc {
namespace rpc {

enum class RpcCallError {
    Success,
    NetworkTimeout,
    FunctionNotFound,
    FunctionNameMismatch
};

enum class RpcReturnCode : uint8_t {
    Success,
    FunctionNotFound
};

template <typename T>
struct RpcReturnType {
    using Type = T;
};

template <>
struct RpcReturnType<void> {
    using Type = uint8_t;
};

template <typename R>
using RpcReturnWrapper = std::tuple<RpcReturnCode, typename RpcReturnType<R>::Type>;

template <class ...Params>
using RpcFuncArgsWrapper = std::tuple<Params...>;

}
}
