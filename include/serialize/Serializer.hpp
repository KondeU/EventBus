#pragma once

#include "SerializeProcesser.hpp"

namespace au {
namespace serialize {

template <typename Processor>
// Use the template Processor type instead of
// virtual function implementations, because that
// virtual function cannot implement the scenarios
// with variable parameters and variable parameter
// types, and the template is determined at compile
// time, it also can reduce the runtime cost.
class Serializer {
public:
    template <class ...Args>
    inline void Serialize(std::string& data, const Args& ...args)
    {
        processor.Serialize(data, args...);
        processor.Reset();
    }

    template <class ...Args>
    inline void Deserialize(const std::string& data, Args& ...args)
    {
        processor.Deserialize(data, args...);
        processor.Reset();
    }

private:
    static_assert(std::is_base_of<SerializeProcesser, Processor>::value,
        "Processor must derived from SerializeProcesser!");

    Processor processor;
};

using XmlSerializer = Serializer<CerealSerializeProcesser<
    cereal::XMLInputArchive, cereal::XMLOutputArchive>>;
using JsonSerializer = Serializer<CerealSerializeProcesser<
    cereal::JSONInputArchive, cereal::JSONOutputArchive>>;
using BinSerializer = Serializer<CerealSerializeProcesser<
    cereal::BinaryInputArchive, cereal::BinaryOutputArchive>>;
using NetBinSerializer = Serializer<CerealSerializeProcesser<
    cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>>;
using MsgBinSerializer = Serializer<MsgpackSerializeProcesser>;

}
}
