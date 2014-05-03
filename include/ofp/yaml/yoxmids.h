#ifndef OFP_YAML_YOXMIDS_H_
#define OFP_YAML_YOXMIDS_H_

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMID> {
    static void output(const ofp::OXMID &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
      out << value.type();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::OXMID &value)
    {
        //if (!value.parse(scalar)) {
        //    return "Invalid OXMID.";
        //}
        return "";
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
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YOXMIDS_H_
