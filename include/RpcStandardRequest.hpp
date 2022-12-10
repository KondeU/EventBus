#pragma once

#include "MultiServerSupportBase.hpp"
#include "serializer/FunctionSerializer.hpp"
#include "RpcBasicTemplate.hpp"

namespace tirpc {

class RpcProcessRequest : public MultiServerSupportBase {
public:
    RpcProcessRequest()
    {
        Communicator().ResetInstInvalid(requester);
    }

    bool ConnectNetwork(const std::string& serverIp, int serverPort)
    {
        if (!Communicator().IsInstInvalid(requester)) {
            return false;
        }

        std::string addr = serverIp + ":" + std::to_string(serverPort);
        requester = Communicator().CreateRequester(addr);
        if (Communicator().IsInstInvalid(requester)) {
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
        if (Communicator().IsInstInvalid(requester)) {
            return false;
        }

        Communicator().DestroyInstance(Communicator().MakeInstValue(requester));
        Communicator().ResetInstInvalid(requester);

        return true;
    }

    void SetAutoReconnectNetwork(bool enable)
    {
        // Reconnect network automatically when
        // network timeout if enable is true.
        // See CallFunc in detail.
        autoReconnectNetwork = enable;
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
                //     problems in this short time.
                Communicator().DestroyInstance(
                    Communicator().MakeInstValue(requester));
                Communicator().ResetInstInvalid(requester);
                requester = Communicator().CreateRequester(serverAddress);
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
        case communicator::CommunicationCode::Success:
            return rpc::RpcCallError::Success; // Only successfully received.
        case communicator::CommunicationCode::ReceiveTimeout:
            return rpc::RpcCallError::NetworkTimeout;
        }
        // It is not possible to run here. Only used to avoid compilation warning.
        return rpc::RpcCallError::NetworkTimeout;
    }

private:
    static constexpr int RpcTimeout = 1000; // 1s

    bool autoReconnectNetwork = true;
    std::string serverAddress;

    serializer::FunctionSerializer serializer;
    communicator::RequesterInst requester;
};

}
