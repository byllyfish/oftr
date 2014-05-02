#ifndef OFP_YAML_YACTIONIDS_H_
#define OFP_YAML_YACTIONIDS_H_

#include "ofp/actionidlist.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::ActionID> {
    static void output(const ofp::ActionID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
      out << value.type();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::ActionID &value)
    {
        //if (!value.parse(scalar)) {
        //    return "Invalid ActionID.";
        //}
        return "";
    }
};


template <>
struct SequenceTraits<ofp::ActionIDRange> {

  static size_t size(IO &io, ofp::ActionIDRange &range) {
    return range.itemCount();
  }

  static ofp::ActionID &
  element(IO &io, ofp::ActionIDRange &range, size_t index) {
    ofp::ActionIDIterator iter = range.nthItem(index);
    return RemoveConst_cast(*iter);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YACTIONIDS_H_
