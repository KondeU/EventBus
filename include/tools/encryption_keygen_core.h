#pragma once

#include <string>

namespace au {
namespace commtool {

class EncryptionKeygen {
public:
    bool Generate(std::string& publicKey, std::string& secretKey);
};

}
}
