#pragma once

#include "SerializeHeaders.hpp"

namespace tibus {
namespace serialize {

// An empty base class, only
// be used for type checking.
class SerializeProcesser {};

class CerealSerializeProcesser : public SerializeProcesser {
public:
    template <class ...Args>
    void Serialize(std::string& data, const Args& ...args)
    {
        {
            OutputArchive archive(ss);
            archive(args...);
        }
        data = ss.str();
    }

    template <class ...Args>
    void Deserialize(const std::string& data, Args& ...args)
    {
        ss.str(data);
        {
            InputArchive archive(ss);
            archive(args...);
        }
    }

    void Reset()
    {
        ss.str("");
    }

private:
    using OutputArchive = cereal::PortableBinaryOutputArchive;
    using InputArchive = cereal::PortableBinaryInputArchive;

    std::stringstream ss;
};

class MsgpackSerializeProcesser : public SerializeProcesser {
public:
    template <class ...Args>
    void Serialize(std::string& data, const Args& ...args)
    {
        if (sizeof...(args)) {
            // C++17: if constexpr
            SerializeElement(args...);
        }
        data = data.replace(data.begin(), data.end(), sb.data(), sb.size());
    }

    template <class ...Args>
    void Deserialize(const std::string& data, Args& ...args)
    {
        size_t offset = 0; // Used by DeserializeElement.
        sb.write(data.data(), data.size());
        if (sizeof...(args)) {
            // C++17: if constexpr
            DeserializeElement(offset, args...);
        }
    }

    void Reset()
    {
        sb.clear();
    }

private:
    template <class E0, class ...Es>
    inline void SerializeElement(const E0& e0, const Es& ...es)
    {
        msgpack::pack(sb, e0);
        SerializeElement(es...);
    }

    template <class E0>
    inline void SerializeElement(const E0& e0)
    {
        msgpack::pack(sb, e0);
    }

    template <class E0, class ...Es>
    inline void DeserializeElement(size_t& offset, E0& e0, Es& ...es)
    {
        
        auto handle = msgpack::unpack(sb.data(), sb.size(), offset);
        auto object = handle.get();
        object.convert(e0);
        DeserializeElement(offset, es...);
    }

    template <class E0>
    inline void DeserializeElement(size_t& offset, E0& e0)
    {
        auto handle = msgpack::unpack(sb.data(), sb.size(), offset);
        auto object = handle.get();
        object.convert(e0);
    }

    msgpack::sbuffer sb;
};

////////////////////////////////////////////////////////////
// For custom structures, if you want to serialize, you need
// to apply this macro in the structure, and the the macro
// will automatically add serialization-related template
// functions for serialization.
// For example:
//  struct A {
//      int a, b;
//      double c;
//      SERIALIZE_METHOD(a, b, c)
//  };
//  class B {
//  public:
//      SERIALIZE_METHOD(a, b, c)
//  private:
//      int a, b;
//      double c;
//  };

#define SERIALIZE_METHOD_CEREAL(...)  \
template <class Archive>              \
void serialize(Archive& archive)      \
{                                     \
    archive(__VA_ARGS__);             \
}

#define SERIALIZE_METHOD_MSGPACK(...) \
MSGPACK_DEFINE(__VA_ARGS__)

#define SERIALIZE_METHOD(...)         \
SERIALIZE_METHOD_CEREAL (...)         \
SERIALIZE_METHOD_MSGPACK(...)
////////////////////////////////////////////////////////////

}
}
