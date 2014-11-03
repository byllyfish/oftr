#ifndef OFP_YAML_YINSTRUCTIONIDS_H_
#define OFP_YAML_YINSTRUCTIONIDS_H_

#include "ofp/instructionidlist.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/yaml/yinstructiontype.h"

namespace ofp {
namespace detail {

struct InstructionIDInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::InstructionID> {
  static void output(const ofp::InstructionID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ScalarTraits<ofp::InstructionType>::output(value.type(), ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::InstructionID &value) {
    ofp::InstructionType type;
    StringRef result =
        ScalarTraits<ofp::InstructionType>::input(scalar, ctxt, type);
    if (result.empty()) {
      value = ofp::InstructionID{type, 0};
    }

    return result;
  }
};

template <>
struct ScalarTraits<ofp::detail::InstructionIDInserter> {
  static void output(const ofp::detail::InstructionIDInserter &value,
                     void *ctxt, llvm::raw_ostream &out) {}

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::detail::InstructionIDInserter &value) {
    ofp::InstructionIDList &list = Ref_cast<ofp::InstructionIDList>(value);

    ofp::InstructionID id;
    StringRef result =
        ScalarTraits<ofp::InstructionID>::input(scalar, ctxt, id);
    if (result.empty()) {
      list.add(id);
    }
    return result;
  }
};

template <>
struct SequenceTraits<ofp::InstructionIDRange> {
  using iterator = ofp::InstructionIDIterator;

  static iterator begin(IO &io, ofp::InstructionIDRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::InstructionIDRange &range) {
    return range.end();
  }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }

  static const bool flow = true;
};

template <>
struct SequenceTraits<ofp::InstructionIDList> {
  static size_t size(IO &io, ofp::InstructionIDList &list) { return 0; }

  static ofp::detail::InstructionIDInserter &element(
      IO &io, ofp::InstructionIDList &list, size_t index) {
    return Ref_cast<ofp::detail::InstructionIDInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YINSTRUCTIONIDS_H_
