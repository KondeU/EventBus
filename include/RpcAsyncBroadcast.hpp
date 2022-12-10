#pragma once

#include "MultiServerSupportBase.hpp"
#include "serializer/FunctionSerializer.hpp"
#include "RpcBasicTemplate.hpp"

namespace tirpc {

class RpcAsyncBroadcast : public MultiServerSupportBase {
public:
    enum class Role {
        None,
        Server,
        Client
    };

    RpcAsyncBroadcast()
    {
        Communicator().ResetInstInvalid(responder);
        Communicator().ResetInstInvalid(requester);
        Communicator().ResetInstInvalid(publisher);
        Communicator().ResetInstInvalid(subscriber);

        procResp = // NB: Caution that the life cycle of procResp!
        [this](const std::string& request, std::string& respond)
        {
            ResponseProcess(request, respond);
        };
        procSub = // NB: Caution that the life cycle of procSub!
        [this](const std::string& envelope, const std::string& content)
        {
            SubscribeProcess(envelope, content);
        };
        procSubCb = // NB: Caution that the life cycle of procSubCb!
        [this](bool receivedSuccess)
        {
            if (receivedSuccess) {
                receiveTimeoutCounter = 0;
            } else {
                receiveTimeoutCounter++;
            }
            if (receiveTimeoutCountCallback) {
                receiveTimeoutCountCallback(receiveTimeoutCounter);
            }
        };
    }

    bool Start(Role node, const std::string& ip,
        int callfunc, int broadcast) // port of ReqRep and PubSub
    {
        if (role != Role::None) {
            return false;
        }
        if (node == Role::None) {
            return false;
        }

        receiveTimeoutCounter = 0;

        role = node;
        addrReqRep = ip + ":" + std::to_string(callfunc);
        addrPubSub = ip + ":" + std::to_string(broadcast);

        bool success = true;
        switch (node) {
        default:
        case Role::None:
            // It is not possible to run here.
            // Only used to avoid compilation warning.
            // It has been checked that node is not Role::None.
            return false;

        case Role::Server:
            publisher = Communicator().CreatePublisher(addrPubSub);
            if (Communicator().IsInstInvalid(publisher)) {
                success = false;
            }
            responder = Communicator().CreateResponder(addrReqRep);
            if (Communicator().IsInstInvalid(responder)) {
                success = false;
            } else {
                if (!responder->StartResponse(procResp)) {
                    success = false;
                }
            }
            // !!----- FALL THROUGH -----!!
            // The server is also a client!
            #if ((__cplusplus >= 201703L) ||\
                (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
            [[fallthrough]];
            #endif

        case Role::Client:
            subscriber = Communicator().CreateSubscriber(addrPubSub);
            if (Communicator().IsInstInvalid(subscriber)) {
                success = false;
            } else {
                int timeout = subscriber->SetTimeout(RpcTimeout);
                if (timeout != RpcTimeout) {
                    success = false;
                }
                subscriber->Subscribe("");
                if (!subscriber->StartReceive(procSubCb, procSub)) {
                    success = false;
                }
            }
            requester = Communicator().CreateRequester(addrReqRep);
            if (Communicator().IsInstInvalid(requester)) {
                success = false;
            } else {
                int timeout = requester->SetTimeout(RpcTimeout);
                if (timeout != RpcTimeout) {
                    success = false;
                }
            }
            break;
        }
        return success;
    }

    bool Stop()
    {
        if (role == Role::None) {
            return false;
        }

        bool success = true;
        if (responder) { // [Server]
            // => Only Server role has it.
            if (!responder->StopResponse() ||
                !responder->WaitResponse() ||
                !responder->ResetResponse()) {
                success = false;
            }
        }
        if (subscriber) { // [Client]
            // => Both Server and Client role have it.
            if (!subscriber->StopReceive() ||
                !subscriber->WaitReceive() ||
                !subscriber->ResetReceive()) {
                success = false;
            }
        }

        if (!Communicator().IsInstInvalid(responder)) {
            Communicator().DestroyInstance(
                Communicator().MakeInstValue(responder));
            Communicator().ResetInstInvalid(responder);
        }
        if (!Communicator().IsInstInvalid(requester)) {
            Communicator().DestroyInstance(
                Communicator().MakeInstValue(requester));
            Communicator().ResetInstInvalid(requester);
        }
        if (!Communicator().IsInstInvalid(publisher)) {
            Communicator().DestroyInstance(
                Communicator().MakeInstValue(publisher));
            Communicator().ResetInstInvalid(publisher);
        }
        if (!Communicator().IsInstInvalid(subscriber)) {
            Communicator().DestroyInstance(
                Communicator().MakeInstValue(subscriber));
            Communicator().ResetInstInvalid(subscriber);
        }

        role = Role::None;
        addrReqRep = "";
        addrPubSub = "";
        return success;
    }

    void RegistReceiveTimeoutCallback(std::function<void(int)> callback)
    {
        receiveTimeoutCountCallback = callback;
    }

    void UnregistReceiveTimeoutCallback()
    {
        receiveTimeoutCountCallback = std::function<void(int)>();
    }

    template <typename Func>
    inline void BindFunc(const std::string& name, Func func)
    {
        rpcs[name] = std::bind(&RpcAsyncBroadcast::CallProxy<Func>,
            this, func, std::placeholders::_1);
    }

    template <typename CFunc, typename CType>
    inline void BindFunc(const std::string& name, CFunc func, CType& impl)
    {
        rpcs[name] = std::bind(&RpcAsyncBroadcast::CallProxy<CFunc, CType>,
            this, func, &impl, std::placeholders::_1);
    }

    template <typename ...Args>
    rpc::RpcCallError CallFunc(const std::string& name, const Args& ...args)
    {
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...>
            wrapper = std::make_tuple(args...);

        std::string request;
        fsCfn.Serialize(request, name, wrapper);

        std::string respond;
        switch (requester->Request(request, respond)) {
        case communicator::CommunicationCode::Success:
            break; // Success means the network communication is normal.
        case communicator::CommunicationCode::ReceiveTimeout:
            // Network timeout and disconnect network:
            Communicator().DestroyInstance(
                Communicator().MakeInstValue(requester));
            Communicator().ResetInstInvalid(requester);
            // Network timeout and reconnect network:
            requester = Communicator().CreateRequester(addrReqRep);
            requester->SetTimeout(RpcTimeout);
            // NB: Here we did not do very detailed verification as
            //     in the Stop and Start functions. We assumed that
            //     there would be no problems in this short time...
            // Finally return NetworkTimeout to notify the caller.
            return rpc::RpcCallError::NetworkTimeout;
        }

        std::string retFuncName;
        rpc::RpcReturnCode retReturnCode;
        fsCfn.Deserialize(respond, retFuncName, retReturnCode);
        // Function name mismatch, may be out-of-order calls occurred.
        if (retFuncName != name) {
            return rpc::RpcCallError::FunctionNameMismatch;
        }
        // Only Success or FunctionNotFound is depend by the server execute.
        switch (retReturnCode) {
        case rpc::RpcReturnCode::Success:
            return rpc::RpcCallError::Success;
        case rpc::RpcReturnCode::FunctionNotFound:
            return rpc::RpcCallError::FunctionNotFound;
        }
        // It is not possible to run here.
        // Only used to avoid compilation warning.
        // retReturnCode type is rpc::RpcReturnCode, and
        // rpc::RpcReturnCode only has Success and FunctionNotFound.
        return rpc::RpcCallError::FunctionNotFound;
    }

protected:
    //-- ResponseProcess: Only running on the server.
    void ResponseProcess(const std::string& request, std::string& respond)
    {
        std::string funcName;
        fsRep.Deserialize(request, funcName);
        auto funcIter = rpcs.find(funcName);
        if (funcIter != rpcs.end()) {
            publisher->Publish(funcName, request); // publish to all clients
            fsRep.Serialize(respond, funcName, rpc::RpcReturnCode::Success);
        } else {
            fsRep.Serialize(respond, funcName, rpc::RpcReturnCode::FunctionNotFound);
        }
    } // ResponseProcess: Server(ExecFunc), running in responder thread.

    //-- SubscribeProcess: Only running on the client.
    void SubscribeProcess(const std::string& envelope, const std::string& content)
    {
        // Using envelope to store the function name here,
        // which will cause the data package to become larger,
        // another way is deserialize twice locally(see ResponseProcess).
        auto funcIter = rpcs.find(envelope);
        if (funcIter != rpcs.end()) {
            funcIter->second(content);
        }
    } // SubscribeProcess: Client, running in subscriber thread.

    template <typename Func>
    inline void CallProxy(Func func, const std::string& data)
    {
        CallProxyImpl(func, data);
    }

    template <typename CFunc, typename CType>
    inline void CallProxy(CFunc func, CType* impl, const std::string& data)
    {
        CallProxyImpl(func, impl, data);
    }

    template <class ...Args>
    void CallProxyImpl(void(*func)(Args...), const std::string& data)
    {
        CallProxyImpl(std::function<void(Args...)>(func), data);
    }

    template <typename CImpl, class ...Args>
    void CallProxyImpl(void(CImpl::*func)(Args...), CImpl* impl, const std::string& data)
    {
        auto proxy = [&func, &impl](Args ...args) {
            (impl->*func)(args...);
        };
        CallProxyImpl(std::function<void(Args...)>(proxy), data);
    }

    template <class ...Args>
    void CallProxyImpl(std::function<void(Args...)> func, const std::string& data)
    {
        std::string funcName;
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...> funcArgs;
        fsSub.Deserialize(data, funcName, funcArgs);
        CallInvoke(func, funcArgs);
    }

    template <typename Func, typename ArgsTuple>
    void CallInvoke(Func&& func, ArgsTuple&& argsTuple)
    {
        constexpr auto Size = std::tuple_size<
            typename std::decay<ArgsTuple>::type>::value;
        CallInvokeImpl(std::forward<Func>(func),
            std::forward<ArgsTuple>(argsTuple),
            std::make_index_sequence<Size>{});
    }

    template <typename Func, typename ArgsTuple, std::size_t ...Index>
    void CallInvokeImpl(Func&& func, ArgsTuple&& argsTuple,
        std::index_sequence<Index...>)
    {
        func(std::get<Index>(std::forward<ArgsTuple>(argsTuple))...);
    }

private:
    static constexpr int RpcTimeout = 1000; // 1s (Request and Subscribe)

    int receiveTimeoutCounter = 0; // subscriber receive timeout [Client]
    std::function<void(int)> receiveTimeoutCountCallback;     // [Client]

    std::unordered_map<std::string, // function name
        std::function<void(const std::string&)>> rpcs;

    serializer::FunctionSerializer fsRep; // [Server]
    serializer::FunctionSerializer fsSub; // [Client]
    serializer::FunctionSerializer fsCfn; // [Client]

    communicator::ResponderInst  responder;  // [Server]
    communicator::RequesterInst  requester;  // [Client]

    communicator::PublisherInst  publisher;  // [Server]
    communicator::SubscriberInst subscriber; // [Client]

    // params: request, response, process for responder  [Server]
    std::function<void(const std::string&, std::string&)> procResp;
    // params: envelope, content, process for subscriber [Client]
    std::function<void(const std::string&, const std::string&)> procSub;
    // params: received successfully or not, process for subscriber callback.
    std::function<void(bool)> procSubCb;              // [Client]

    Role role = Role::None;
    std::string addrReqRep;
    std::string addrPubSub;
};

}
