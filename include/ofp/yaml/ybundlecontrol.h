// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YBUNDLECONTROL_H_
#define OFP_YAML_YBUNDLECONTROL_H_

#include "ofp/bundlecontrol.h"
#include "ofp/yaml/ybundleproperty.h"

namespace llvm {
namespace yaml {

// type: OFPT_BUNDLE_CONTROL
// msg:
//   bundle_id: <UInt32>  { Required }
//   type: <UInt16>       { Required }
//   flags: <UInt16>      { Required }
//   properties: [ <Property> ]

template <>
struct MappingTraits<ofp::BundleControl> {
  static void mapping(IO &io, ofp::BundleControl &msg) {
    io.mapRequired("bundle_id", msg.bundleId_);
    io.mapRequired("type", msg.ctrlType_);
    io.mapRequired("flags", msg.flags_);

    ofp::PropertyRange props = msg.properties();
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::BundlePropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::BundleControlBuilder> {
  static void mapping(IO &io, ofp::BundleControlBuilder &msg) {
    io.mapRequired("bundle_id", msg.msg_.bundleId_);
    io.mapRequired("type", msg.msg_.ctrlType_);
    io.mapRequired("flags", msg.msg_.flags_);

    ofp::PropertyList props;
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::BundlePropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUNDLECONTROL_H_
