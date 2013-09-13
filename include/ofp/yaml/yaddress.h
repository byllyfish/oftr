#ifndef OFP_YAML_YADDRESS_H
#define OFP_YAML_YADDRESS_H

#include "ofp/yaml/yllvm.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/enetaddress.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<ofp::IPv4Address> {
    static void output(const ofp::IPv4Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv4Address &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid IPv4 address.";
        }
        return "";
    }
};

template <>
struct ScalarTraits<ofp::IPv6Address> {
    static void output(const ofp::IPv6Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv6Address &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid IPv6 address.";
        }
        return "";
    }
};

template <>
struct ScalarTraits<ofp::EnetAddress> {
    static void output(const ofp::EnetAddress &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::EnetAddress &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid Ethernet address.";
        }
        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YADDRESS_H
