#ifndef OFP_YAML_YSMALLCSTRING_H
#define OFP_YAML_YSMALLCSTRING_H

#include "ofp/smallcstring.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <size_t Size>
struct ScalarTraits<ofp::SmallCString<Size>> {
    static void output(const ofp::SmallCString<Size> &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::SmallCString<Size> &value)
    {
    	if (scalar.size() > value.capacity()) {
    		return std::string("Length > ") + std::to_string(value.capacity());
    	}
        value = scalar;

        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YSMALLCSTRING_H
