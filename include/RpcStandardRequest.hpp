#pragma once

#include "Serializer.hpp"
#include "CommunicateContext.hpp"
#include "RpcBasicTemplate.hpp"

namespace au {
namespace rpc {

class RpcStandardRequest {
public:
    bool ConnectNetwork(const std::string& serverIp, int serverPort)
    {
        if (requester != nullptr) {
            return false;
        }

        std::string addr = serverIp + ":" + std::to_string(serverPort);
        requester = communicate::CommunicateContext::GetReference()
            .Create<communicate::Requester>(addr);
        if (requester == nullptr) {
            return false;
        }

        int timeout = requester->SetTimeout(RpcTimeout);
        if (timeout != RpcTimeout) {
            DisconnectNetwork();
            return false;
        }

        serverAddress = addr; // Save the address for use when reconnecting.
        return true;
    }

    bool DisconnectNetwork()
    {
        if (requester == nullptr) {
            return false;
        }

        communicate::CommunicateContext::GetReference().Destroy(requester);

        requester = nullptr;
        return true;
    }

    template <typename T>
    struct CallReturn {
        rpc::RpcCallError error;
        typename rpc::RpcReturnType<T>::Type value;
    };

    template <typename Retv, typename ...Args>
    CallReturn<Retv> CallFunc(const std::string& name, const Args& ...args)
    {
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...>
            wrapper = std::make_tuple(args...);

        std::string request;
        serializer.Serialize(request, name, wrapper);

        std::string respond;
        rpc::RpcCallError error = CallRemote(request, respond);
        // Filtering error, only Success needs to be parsed
        // for further confirmation, see CallRemote.
        if (error == rpc::RpcCallError::NetworkTimeout) {
            // Reconnect network automatically when network timeout.
            if (autoReconnectNetwork) {
                // NB: Here we did not do very detailed verification
                //     as in the ConnectNetwork and DisconnectNetwork
                //     functions. We assumed that there would be no
                //     problems in this short time...
                communicate::CommunicateContext::GetReference().Destroy(requester);
                requester = communicate::CommunicateContext::GetReference()
                    .Create<communicate::Requester>(serverAddress);
                requester->SetTimeout(RpcTimeout);
            }
            return { error, {} };
        }

        std::string retFuncName;
        rpc::RpcReturnWrapper<void> retWrapVoid;
        serializer.Deserialize(respond, retFuncName, retWrapVoid);
        // Function name mismatch, may be out-of-order calls occurred.
        if (retFuncName != name) {
            return { rpc::RpcCallError::FunctionNameMismatch, {} };
        }
        // Filtering error, only rpc::RpcReturnCode::Success needs to be parsed.
        if (std::get<rpc::RpcReturnCode>(retWrapVoid)
            == rpc::RpcReturnCode::FunctionNotFound) {
            return { rpc::RpcCallError::FunctionNotFound, {} };
        }

        CallReturn<Retv> ret{ rpc::RpcCallError::Success, {} };
        if (std::is_same<Retv, void>()) {
            return ret;
        }
        rpc::RpcReturnWrapper<Retv> retWrapRetv;
        serializer.Deserialize(respond, retFuncName, retWrapRetv);
        ret.value = std::get<typename rpc::RpcReturnType<Retv>::Type>(retWrapRetv);
        return ret;
    }

protected:
    inline rpc::RpcCallError CallRemote(
        const std::string& request, std::string& respond)
    {
        switch (requester->Request(request, respond)) {
        case communicate::CommunicationCode::Success:
            return rpc::RpcCallError::Success; // Only successfully received.
        case communicate::CommunicationCode::ReceiveTimeout:
            return rpc::RpcCallError::NetworkTimeout;
        }
        // It is not possible to run here. Only used to avoid compilation warning.
        return rpc::RpcCallError::NetworkTimeout;
    }

private:
    static constexpr int RpcTimeout = 1000; // 1s

    bool autoReconnectNetwork = true;
    std::string serverAddress;

    serialize::NetBinSerializer serializer;
    communicate::Requester* requester = nullptr;
};

}
}
