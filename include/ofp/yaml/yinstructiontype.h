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
            out << value.enumType();
        }
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::InstructionType &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid instruction type.";
        }

        return "";
    }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YINSTRUCTIONTYPE_H_
