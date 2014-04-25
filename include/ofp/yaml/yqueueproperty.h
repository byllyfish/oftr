#ifndef OFP_YAML_YQUEUEPROPERTY_H_
#define OFP_YAML_YQUEUEPROPERTY_H_

namespace ofp {
namespace detail {

struct QueuePropertyItem {};
struct QueuePropertyRange {};
struct QueuePropertyList {
  int begin() const { return 0; }
  int end() const { return 0; }
};
struct QueuePropertyInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::QueuePropertyItem> {
  static void mapping(IO &io, ofp::detail::QueuePropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = reinterpret_cast<PropertyIterator::Element &>(item);

    if (elem.type() == QueuePropertyExperimenter::type()) {
      auto p = elem.property<QueuePropertyExperimenter>();

      UInt32 experimenter = p.experimenter();
      ByteRange data = p.value();

      io.mapRequired("experimenter", experimenter);
      io.mapRequired("value", data);

    } else {
      log::debug("Unsupported QueuePropertyItem");
      UInt32 value = 0;
      io.mapRequired("UNKNOWN", value);
    }
  }
};

template <>
struct MappingTraits<ofp::detail::QueuePropertyInserter> {
  static void mapping(IO &io, ofp::detail::QueuePropertyInserter &inserter) {
    using namespace ofp;

    PropertyList &props = reinterpret_cast<PropertyList &>(inserter);

    UInt32 experimenter;
    io.mapRequired("experimenter", experimenter);

    ByteList data;
    io.mapRequired("value", data);

    props.add(QueuePropertyExperimenter{experimenter, data});
  }
};


template <>
struct SequenceTraits<ofp::detail::QueuePropertyRange> {

  static size_t size(IO &io, ofp::detail::QueuePropertyRange &props) {
    ofp::PropertyRange &p = reinterpret_cast<ofp::PropertyRange &>(props);
    return p.itemCountIf([](const ofp::PropertyRange::Element &item){ return item.type() == ofp::QueuePropertyExperimenter::type(); });
  }

  static ofp::detail::QueuePropertyItem &element(IO &io, ofp::detail::QueuePropertyRange &props,
                                          size_t index) {
    ofp::log::debug("queue property yaml item", index);
    ofp::PropertyRange &p = reinterpret_cast<ofp::PropertyRange &>(props);
    //ofp::PropertyIterator iter = p.begin();
    //for (size_t i = 0; i < index; ++i) ++iter;
    ofp::PropertyIterator iter = p.nthItemIf(index, [](const ofp::PropertyRange::Element &item){ return item.type() == ofp::QueuePropertyExperimenter::type(); });
    const ofp::PropertyIterator::Element &elem = *iter;
    return reinterpret_cast<ofp::detail::QueuePropertyItem &>(RemoveConst_cast(elem));
  }
};

template <>
struct SequenceTraits<ofp::detail::QueuePropertyList> {

  static size_t size(IO &io, ofp::detail::QueuePropertyList &list) { return 0; }

  static ofp::detail::QueuePropertyInserter &element(IO &io, ofp::detail::QueuePropertyList &list,
                                            size_t index) {
    return reinterpret_cast<ofp::detail::QueuePropertyInserter &>(list);
  }
};

} // namespace yaml
} // namespace llvm

#endif // OFP_YAML_YQUEUEPROPERTY_H_
