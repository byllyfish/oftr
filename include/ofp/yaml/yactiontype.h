#ifndef OFP_YAML_YACTIONTYPE_H_
#define OFP_YAML_YACTIONTYPE_H_

#include "ofp/actiontype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::ActionType> {
    static void output(const ofp::ActionType &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        auto info = value.lookupInfo();
        if (info) {
            out << info->name;
        } else {
            out << llvm::format("0x%04X", value.enumType());
        }
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::ActionType &value)
    {
        if (!value.parse(scalar)) {
            unsigned long long num;
            if (llvm::getAsUnsignedInteger(scalar, 0, num) == 0) {
                value.setNative(num);
                return "";
            }
            return "Invalid action type.";
        }

        return "";
    }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YACTIONTYPE_H_
