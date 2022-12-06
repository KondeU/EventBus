#pragma once

#include <thread>
#include <zmq_addon.hpp>
#include "Communicator.hpp"

namespace au {
namespace communicate {

class Broker : public Communicator {
public:
    void Pause()
    {
        if (brokerRunning) {
            const std::string command = "PAUSE";
            // NB: PAUSE will stop Broker's message forwarding,
            //     but will not stop Broker's message receiving,
            //     so messages will accumulate on the Broker side,
            //     and it will cause high CPU load!
            controlCP.send(zmq::const_buffer(command.c_str(),
                command.length()), zmq::send_flags::none);
        }
    }

    void Resume()
    {
        if (brokerRunning) {
            const std::string command = "RESUME";
            controlCP.send(zmq::const_buffer(command.c_str(),
                command.length()), zmq::send_flags::none);
        }
    }

    void Terminate()
    {
        if (brokerRunning) {
            const std::string command = "TERMINATE";
            controlCP.send(zmq::const_buffer(command.c_str(),
                command.length()), zmq::send_flags::none);
            brokerRunning = false;
            // Terminate related threads.
            if (proxyThread.joinable()) {
                proxyThread.join();
            }
            if (captureThread.joinable()) {
                captureThread.join();
            }
        }
    }

    void SetCaptureCallback(const std::function<
        void(const std::vector<std::string>&)>& callback)
    {
        captureCallback = callback;
    }

    void SetErrorCallback(const std::function<
        void(int, const std::string&)>& callback)
    {
        errorCallback = callback;
    }

private:
    friend class CommunicateContext;

    explicit Broker(zmq::context_t& context) : context(context)
        , xsub(context, zmq::socket_type::xsub)
        , xpub(context, zmq::socket_type::xpub)
        , control(context, zmq::socket_type::pair)
        , capture(context, zmq::socket_type::pair)
        , controlCP(context, zmq::socket_type::pair)
        , captureCP(context, zmq::socket_type::pair)
    {
    }

    bool Init(const std::string& xsubAddr, const std::string& xpubAddr,
              bool isCapture = false, std::vector<std::string> encryption = {})
    {
        static size_t num = 0;
        const std::string prefix = "inproc://__internal_broker_";
        const std::string bcon = prefix + std::to_string(num);
        const std::string bcap = prefix + std::to_string(num);
        num++; // There may be more than one broker in a process.

        try {
            control.bind(bcon);
            controlCP.connect(bcon);

            if (isCapture) {
                capture.bind(bcap);
                captureCP.connect(bcap);
            }

            // Setup encryption.
            SetupEncryption(xsub, encryption);
            SetupEncryption(xpub, encryption);

            xsub.bind(xsubAddr);
            xpub.bind(xpubAddr);
        } catch (zmq::error_t) {
            return false;
        }

        // Capture thread.
        if (isCapture) {
            captureCP.set(zmq::sockopt::rcvtimeo, 1000);
            captureThread = std::thread([this]() {
                while (brokerRunning) {
                    std::vector<zmq::message_t> messages;
                    (void)zmq::recv_multipart(captureCP,
                        std::back_inserter(messages));
                    if (messages.size()) {
                        std::vector<std::string> extracted;
                        extracted.resize(messages.size());
                        for (size_t n = 0; n < messages.size(); n++) {
                            extracted[n] = extracted[n].replace(
                                extracted[n].begin(), extracted[n].end(),
                                static_cast<char*>(messages[n].data()),
                                messages[n].size());
                        }
                        if (captureCallback) {
                            captureCallback(extracted);
                        }
                    }
                }
            });
        }

        // Broker thread.
        proxyThread = std::thread([this, isCapture]() {
            zmq::socket_ref captureRef = isCapture ?
                capture : zmq::socket_ref();
            std::string errorString;
            int errorNumber = 0;
            try {
                zmq::proxy_steerable(xsub, xpub,
                    captureRef, control);
            } catch (zmq::error_t error) {
                errorString = error.what();
                errorNumber = error.num();
                if (errorCallback) {
                    errorCallback(errorNumber, errorString);
                }
            }
            while (brokerRunning) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(1));
            }
        });

        return true;
    }

    zmq::context_t& context;
    zmq::socket_t xsub, xpub;
    zmq::socket_t control, controlCP;
    zmq::socket_t capture, captureCP;

    std::function<void(int, const std::string&)>
        errorCallback; // It may be called in proxyThread.
    std::function<void(std::vector<std::string>&)>
        captureCallback; // It will be called in captureThread.

    bool brokerRunning = true;
    std::thread captureThread;
    std::thread proxyThread;
};

}
}
