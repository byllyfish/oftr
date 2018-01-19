// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YQUEUEDESC_H_
#define OFP_YAML_YQUEUEDESC_H_

#include "ofp/queuedesc.h"
#include "ofp/yaml/yqueuedescproperty.h"

namespace llvm {
namespace yaml {

const char *const kQueueDescSchema = R"""({Struct/QueueDesc}
port_no: PortNumber
queue_id: QueueNumber
min_rate: UInt16
max_rate: UInt16
properties: !opt [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::QueueDesc> {
  static void mapping(IO &io, ofp::QueueDesc &msg) {
    io.mapRequired("port_no", msg.portNo_);
    io.mapRequired("queue_id", msg.queueId_);

    ofp::PropertyRange props = msg.properties();

    Hex16 minRate = props.value<ofp::QueueDescPropertyMinRate>();
    Hex16 maxRate = props.value<ofp::QueueDescPropertyMaxRate>();
    io.mapRequired("min_rate", minRate);
    io.mapRequired("max_rate", maxRate);

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::QueueDescPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::QueueDescBuilder> {
  static void mapping(IO &io, ofp::QueueDescBuilder &msg) {
    using namespace ofp;

    io.mapRequired("port_no", msg.desc_.portNo_);
    io.mapRequired("queue_id", msg.desc_.queueId_);

    UInt16 minRate = 0;
    UInt16 maxRate = 0;
    io.mapRequired("min_rate", minRate);
    io.mapRequired("max_rate", maxRate);

    PropertyList props;
    if (minRate != 0xffff)
      props.add(QueueDescPropertyMinRate{minRate});
    if (maxRate != 0xffff)
      props.add(QueueDescPropertyMaxRate{maxRate});

    PropertyList &p = props;
    ofp::detail::QueueDescPropertyList &qp =
        Ref_cast<ofp::detail::QueueDescPropertyList>(p);
    io.mapOptional("properties", qp);
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEDESC_H_
