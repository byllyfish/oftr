#ifndef OFP_YAML_YINSTRUCTIONTYPE_H_
#define OFP_YAML_YINSTRUCTIONTYPE_H_

#include "ofp/instructiontype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::InstructionType> {
    static void output(const ofp::InstructionType &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        auto info = value.lookupInfo();
        if (info) {
            out << info->name;
        } else {
            out << llvm::format("0x%04X", value.enumType());
        }
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::InstructionType &value)
    {
        if (!value.parse(scalar)) {
            unsigned long long num;
            if (llvm::getAsUnsignedInteger(scalar, 0, num) == 0) {
                value.setNative(ofp::UInt16_narrow_cast(num));
                return "";
            }
            return "Invalid instruction type.";
        }

        return "";
    }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YINSTRUCTIONTYPE_H_
