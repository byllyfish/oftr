// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YBUNDLEADDMESSAGE_H_
#define OFP_YAML_YBUNDLEADDMESSAGE_H_

#include "ofp/bundleaddmessage.h"
#include "ofp/yaml/ybundleproperty.h"

namespace llvm {
namespace yaml {

const char *const kBundleAddMessageSchema = R"""({Message/BundleAddMessage}
type: BUNDLE_ADD_MESSAGE
msg:
  bundle_id: UInt32
  flags: [BundleFlags]
  message: Message
  properties: [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::BundleAddMessage> {
  static void mapping(IO &io, ofp::BundleAddMessage &msg) {
    io.mapRequired("bundle_id", msg.bundleId_);
    io.mapRequired("flags", msg.flags_);

    auto data = msg.message();
    ofp::Message message{data.data(), data.size()};
    message.transmogrify();

    ofp::yaml::DecodeRecursively(io, "message", &message);

    ofp::PropertyRange props = msg.properties();
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::BundlePropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::BundleAddMessageBuilder> {
  static void mapping(IO &io, ofp::BundleAddMessageBuilder &msg) {
    io.mapRequired("bundle_id", msg.msg_.bundleId_);
    io.mapRequired("flags", msg.msg_.flags_);

    ofp::yaml::EncodeRecursively(io, "message", msg.message_);

    ofp::PropertyList props;
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::BundlePropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUNDLEADDMESSAGE_H_
