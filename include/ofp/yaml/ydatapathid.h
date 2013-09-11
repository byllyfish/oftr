#ifndef OFP_YAML_YDATAPATHID_H
#define OFP_YAML_YDATAPATHID_H

#include "ofp/datapathid.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<ofp::DatapathID> {
    static void output(const ofp::DatapathID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::DatapathID &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid DatapathID.";
        }
        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YDATAPATHID_H
