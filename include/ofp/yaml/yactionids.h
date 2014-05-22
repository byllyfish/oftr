#ifndef OFP_YAML_YACTIONIDS_H_
#define OFP_YAML_YACTIONIDS_H_

#include "ofp/actionidlist.h"
#include "ofp/yaml/yactions.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::ActionID> {
    static void output(const ofp::ActionID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
      ScalarTraits<ofp::ActionType>::output(value.type(), ctxt, out);
      //out << value.type();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::ActionID &value)
    {
      ofp::ActionType type;
      StringRef result = ScalarTraits<ofp::ActionType>::input(scalar, ctxt, type);
      if (result.empty()) {
        value = ofp::ActionID{type, 0};
      }

      return result;
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

  static const bool flow = true;
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YACTIONIDS_H_
