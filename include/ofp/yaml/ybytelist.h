#ifndef OFP_YAML_YBYTELIST_H
#define OFP_YAML_YBYTELIST_H

#include "ofp/yaml/yllvm.h"
#include "ofp/bytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<ofp::ByteRange> {

    static void output(const ofp::ByteRange &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << ofp::RawDataToHex(value.data(), value.size());
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::ByteRange &value)
    {
        return "Not supported.";
    }
};

template <>
struct ScalarTraits<ofp::ByteList> {

    static void output(const ofp::ByteList &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << ofp::RawDataToHex(value.data(), value.size());
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::ByteList &value)
    {
        value.resize(scalar.size() / 2 + 2);
        bool error = false;
        size_t actualSize = ofp::HexToRawData(scalar, value.mutableData(),
                                              value.size(), &error);
        value.resize(actualSize);
        if (error)
            return "Invalid hexadecimal text.";
        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YBYTELIST_H