#ifndef OFP_YAML_YINSTRUCTIONIDS_H_
#define OFP_YAML_YINSTRUCTIONIDS_H_

#include "ofp/instructionidlist.h"
#include "ofp/yaml/yconstants.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::InstructionID> {
    static void output(const ofp::InstructionID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
      out << value.type();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::InstructionID &value)
    {
        //if (!value.parse(scalar)) {
        //    return "Invalid InstructionID.";
        //}
        return "";
    }
};


template <>
struct SequenceTraits<ofp::InstructionIDRange> {

  static size_t size(IO &io, ofp::InstructionIDRange &range) {
    return range.itemCount();
  }

  static ofp::InstructionID &
  element(IO &io, ofp::InstructionIDRange &range, size_t index) {
    ofp::InstructionIDIterator iter = range.nthItem(index);
    return RemoveConst_cast(*iter);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YINSTRUCTIONIDS_H_
