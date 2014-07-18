#ifndef OFP_YAML_YQUEUE_
#define OFP_YAML_YQUEUE_

#include "ofp/yaml/yqueueproperty.h"

namespace ofp {
namespace detail {

struct QueueInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

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

    //PropertyRange &p = props;
    io.mapRequired("properties", Ref_cast<ofp::detail::QueuePropertyRange>(props));

    //ofp::detail::QueuePropertyRange &qp = reinterpret_cast<ofp::detail::QueuePropertyRange &>(p);
    //io.mapRequired("properties", qp);
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
    msg.setProperties(props.toRange());
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
  using iterator = ofp::QueueIterator;

  static iterator begin(IO &io, ofp::QueueRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::QueueRange &range) {
    return range.end();
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
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
