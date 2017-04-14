#ifndef OFP_YAML_YSTAT_H_
#define OFP_YAML_YSTAT_H_

#include "ofp/statbuilder.h"
#include "ofp/yaml/ymatch.h"

namespace ofp {
namespace detail {

class StatBuilderInserter {
 public:
  StatBuilderInserter(llvm::yaml::IO &io, StatBuilder &builder,
                       OXMFullType type)
      : io_(io), builder_(builder), type_{type} {
  }

  template <class ValueType>
  void visit() {
    assert(!type_.hasMask());

    typename ValueType::NativeType value;
    io_.mapRequired("value", value);
    builder_.add(ValueType{value});
  }

 private:
  llvm::yaml::IO &io_;
  StatBuilder &builder_;
  OXMFullType type_;
};

class StatBuilderItem;

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct SequenceTraits<ofp::StatBuilder> {
  static size_t size(IO &io, ofp::StatBuilder &match) { return 0; }

  static ofp::detail::StatBuilderItem &element(IO &io,
                                                ofp::StatBuilder &match,
                                                size_t index) {
    return Ref_cast<ofp::detail::StatBuilderItem>(match);
  }
};

template <>
struct MappingTraits<ofp::detail::StatBuilderItem> {
  static void mapping(IO &io, ofp::detail::StatBuilderItem &item) {
    ofp::StatBuilder &builder = Ref_cast<ofp::StatBuilder>(item);

    ofp::OXMFullType type;
    io.mapRequired("field", type);

    ofp::OXMInternalID id = type.internalID();
    if (id != ofp::OXMInternalID::UNKNOWN) {
      ofp::detail::StatBuilderInserter inserter{io, builder, type};
      OXMDispatch(id, &inserter);

    } else {
      addUnexpected(io, builder, type);
    }
  }

  static void addUnexpected(IO &io, ofp::StatBuilder &builder, const ofp::OXMFullType &type) {
    log_debug("MappingTraits<StatBuilderItem>: Unexpected match field",
              type);
    ofp::ByteList data;
    io.mapRequired("value", data);

    size_t len = data.size() + (type.experimenter() != 0 ? 4 : 0);
    if (len == type.length()) {
      builder.add(type.type(), type.experimenter(), data);
    } else {
      io.setError("Invalid field length");
      log_debug("Invalid data size:", type);
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YSTAT_H_
