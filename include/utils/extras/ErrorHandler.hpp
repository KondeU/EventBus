#pragma once

#include "TiBus.hpp"

class ErrorHandler : public tibus::common::GlobalSingleton<ErrorHandler> {
public:
    struct BasicLevel {
        static constexpr int Debug       = -1;
        static constexpr int Information =  0;
        static constexpr int Warning     =  1;
        static constexpr int Error       =  2;
        static constexpr int Fatal       =  3;
    };

    struct BasicCode {
        static constexpr int Success =  0;
        static constexpr int Unknown = -1;
    };

    ErrorHandler()
    {
        RecordError(BasicCode::Success, BasicLevel::Information, "Success.");
        RecordError(BasicCode::Unknown, BasicLevel::Error, "Unknown error.");
    }

    void RecordError(int code, int level, const std::string& description)
    {
        errors[code] = std::make_pair(level, description);
    }

    void ThrowError(int code)
    {
        if (callback) {
            callback(errors[code].first, code, errors[code].second);
        }
    }

private:
    std::unordered_map<int, std::pair<int, std::string>> errors;
    std::function<void(int, int, const std::string&)> callback;
};
