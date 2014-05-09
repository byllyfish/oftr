#ifndef OFP_YAML_YOXMIDS_H_
#define OFP_YAML_YOXMIDS_H_

#include "ofp/yaml/yoxmtype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMID> {
    static void output(const ofp::OXMID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
      ScalarTraits<ofp::OXMType>::output(value.type(), ctxt, out);
      //out << value.type();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::OXMID &value)
    {
      ofp::OXMType type;
      StringRef result = ScalarTraits<ofp::OXMType>::input(scalar, ctxt, type);
      if (result.empty()) {
        value = ofp::OXMID{type, 0};
      }

      return result;
    }
};


template <>
struct SequenceTraits<ofp::OXMIDRange> {

  static size_t size(IO &io, ofp::OXMIDRange &range) {
    return range.itemCount();
  }

  static ofp::OXMID &
  element(IO &io, ofp::OXMIDRange &range, size_t index) {
    ofp::OXMIDIterator iter = range.nthItem(index);
    return RemoveConst_cast(*iter);
  }

  static const bool flow = true;
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YOXMIDS_H_
