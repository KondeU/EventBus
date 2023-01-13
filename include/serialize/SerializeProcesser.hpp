#pragma once

#include "SerializeHeaders.hpp"

namespace au {
namespace serialize {

// An empty base class, only
// be used for type checking.
class SerializeProcesser {};

template <
    typename InputArchive = cereal::PortableBinaryInputArchive,
    typename OutputArchive = cereal::PortableBinaryOutputArchive>
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

    // template <>
    void Serialize(std::string& data)
    {
        data.clear();
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

    // template <>
    void Deserialize(const std::string& data)
    {
        (void)data;
    }

    void Reset()
    {
        ss.str("");
    }

private:
    std::stringstream ss;
};

class MsgpackSerializeProcesser : public SerializeProcesser {
public:
    template <class ...Args>
    void Serialize(std::string& data, const Args& ...args)
    {
        SerializeElement(args...);
        data = data.replace(data.begin(), data.end(), sb.data(), sb.size());
    }

    // template <>
    void Serialize(std::string& data)
    {
        data.clear();
    }

    template <class ...Args>
    void Deserialize(const std::string& data, Args& ...args)
    {
        size_t offset = 0;
        sb.write(data.data(), data.size());
        DeserializeElement(offset, args...);
    }

    // template <>
    void Deserialize(const std::string& data)
    {
        (void)data;
    }

    void Reset()
    {
        sb.clear();
    }

private:
    template <class E0, class ...Es>
    inline void SerializeElement(const E0& e0, const Es& ...es)
    {
        SerializeElement(e0);
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
        DeserializeElement(offset, e0);
        DeserializeElement(offset, es...);
    }

    template <class E0>
    inline void DeserializeElement(size_t& offset, E0& e0)
    {
        auto handle = msgpack::unpack(sb.data(), sb.size(), offset);
        auto& object = handle.get();
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

#define SERIALIZE_METHOD_CEREAL(...) \
template <class Archive>             \
void serialize(Archive& archive)     \
{                                    \
    archive(__VA_ARGS__);            \
}
#define GEN_NAME_VALUE_PAIR CEREAL_NVP

#define SERIALIZE_METHOD_MSGPACK(...) \
MSGPACK_DEFINE(__VA_ARGS__)

#define GEN_SERIALIZE_METHOD SERIALIZE_METHOD_CEREAL
#define MSG_SERIALIZE_METHOD SERIALIZE_METHOD_MSGPACK

#define SERIALIZE_METHOD(...)     \
GEN_SERIALIZE_METHOD(__VA_ARGS__) \
MSG_SERIALIZE_METHOD(__VA_ARGS__)
////////////////////////////////////////////////////////////

}
}
