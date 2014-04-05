#ifndef OFP_YAML_YQUEUE_
#define OFP_YAML_YQUEUE_

namespace ofp {
namespace detail {

struct QueueInserter {};

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

//---
// <Queue> := {
//   queue_id: <UInt32>                 #required
//   port: <UInt32>                     #required
//   min_rate: <UInt16>                 #default: 0xffff
//   max_rate: <UInt16>                 #default: 0xffff
//   properties: [ <QueuePropertyX>  ]  #required
// }
//...

template <>
struct MappingTraits<ofp::Queue> {
  static void mapping(IO &io, ofp::Queue &msg) {
    using namespace ofp;

    io.mapRequired("queue_id", msg.queueId_);
    io.mapRequired("port", msg.port_);

    PropertyRange props = msg.properties();

    UInt16 minRate = props.value<QueuePropertyMinRate>();
    UInt16 maxRate = props.value<QueuePropertyMaxRate>();
    io.mapRequired("min_rate", minRate);
    io.mapRequired("max_rate", maxRate);

    PropertyRange &p = props;
    ofp::detail::QueuePropertyRange &qp = reinterpret_cast<ofp::detail::QueuePropertyRange &>(p);
    io.mapRequired("properties", qp);
  }
};

template <>
struct MappingTraits<ofp::QueueBuilder> {
  static void mapping(IO &io, ofp::QueueBuilder &msg) {
    using namespace ofp;

    io.mapRequired("queue_id", msg.queue_.queueId_);
    io.mapRequired("port", msg.queue_.port_);

    UInt16 minRate;
    UInt16 maxRate;
    io.mapRequired("min_rate", minRate);
    io.mapRequired("max_rate", maxRate);

    PropertyList props;
    props.add(QueuePropertyMinRate{minRate});
    props.add(QueuePropertyMaxRate{maxRate});

    PropertyList &p = props;
    ofp::detail::QueuePropertyList &qp = reinterpret_cast<ofp::detail::QueuePropertyList &>(p);
    io.mapOptional("properties", qp);
    msg.setProperties(props);
  }
};

template <>
struct MappingTraits<ofp::detail::QueueInserter> {
  static void mapping(IO &io, ofp::detail::QueueInserter &inserter) {
    using namespace ofp;

    QueueList &queues = reinterpret_cast<QueueList &>(inserter);
    QueueBuilder queue;
    MappingTraits<QueueBuilder>::mapping(io, queue);
    queues.add(queue);
  }
};

template <>
struct SequenceTraits<ofp::QueueRange> {

  static size_t size(IO &io, ofp::QueueRange &queues) {
    return queues.itemCount();
  }

  static ofp::QueueIterator::Element &element(IO &io, ofp::QueueRange &queues,
                                          size_t index) {
    ofp::log::debug("queue yaml item", index);
    // FIXME
    ofp::QueueIterator iter = queues.begin();
    for (size_t i = 0; i < index; ++i) ++iter;
    return RemoveConst_cast(*iter);
  }
};

template <>
struct SequenceTraits<ofp::QueueList> {

  static size_t size(IO &io, ofp::QueueList &list) { return 0; }

  static ofp::detail::QueueInserter &element(IO &io, ofp::QueueList &list,
                                            size_t index) {
    return reinterpret_cast<ofp::detail::QueueInserter &>(list);
  }
};




}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YQUEUE_
