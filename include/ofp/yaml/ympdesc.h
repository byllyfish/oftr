// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPDESC_H_
#define OFP_YAML_YMPDESC_H_

#include "ofp/mpdesc.h"

namespace llvm {
namespace yaml {

const char *const kMPDescSchema = R"""({Multipart/Desc}
mfr_desc: Str255
hw_desc: Str255
sw_desc: Str255
serial_num: Str31
dp_desc: Str255
)""";

template <>
struct MappingTraits<ofp::MPDesc> {
  static void mapping(IO &io, ofp::MPDesc &body) {
    io.mapRequired("mfr_desc", body.mfrDesc_);
    io.mapRequired("hw_desc", body.hwDesc_);
    io.mapRequired("sw_desc", body.swDesc_);
    io.mapRequired("serial_num", body.serialNum_);
    io.mapRequired("dp_desc", body.dpDesc_);
  }
};

template <>
struct MappingTraits<ofp::MPDescBuilder> {
  static void mapping(IO &io, ofp::MPDescBuilder &body) {
    io.mapRequired("mfr_desc", body.msg_.mfrDesc_);
    io.mapRequired("hw_desc", body.msg_.hwDesc_);
    io.mapRequired("sw_desc", body.msg_.swDesc_);
    io.mapRequired("serial_num", body.msg_.serialNum_);
    io.mapRequired("dp_desc", body.msg_.dpDesc_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPDESC_H_
