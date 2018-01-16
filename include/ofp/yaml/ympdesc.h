// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPDESC_H_
#define OFP_YAML_YMPDESC_H_

#include "ofp/mpdesc.h"

namespace llvm {
namespace yaml {

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
