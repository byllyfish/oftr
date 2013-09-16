#ifndef OFP_YAML_YLLVM_H
#define OFP_YAML_YLLVM_H

// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/Header.h"

#if 0
namespace ofp {    // <namespace ofp>
namespace detail { // <namespace detail>

void readHeader(llvm::yaml::IO &io, Header *header);
void writeHeader(llvm::yaml::IO &io, const Header *header);

inline void readHeader(llvm::yaml::IO &io, Header *header)
{
    OFPType type;
    UInt8 version;
    UInt32 xid;

    // FIXME - is this right? What happens if msg comes first?

    io.mapRequired("type", type);
    io.mapRequired("xid", xid);
    io.mapRequired("version", version);

    header->setType(type);
    header->setVersion(version);
    header->setXid(xid);
}

inline void writeHeader(llvm::yaml::IO &io, const Header *header)
{
    UInt8 version = header->version();
    OFPType type = header->type();
    UInt32 xid = header->xid();

    io.mapRequired("type", type);
    io.mapRequired("version", version);
    io.mapRequired("xid", xid);
}

} // </namespace detail>
} // </namespace ofp>
#endif //0

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<std::string> {
    static void output(const std::string &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value;
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           std::string &value)
    {
        value = scalar;
        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YLLVM_H
