//  ===== ---- ofp/yaml/ymultipartreply.h ------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the llvm::yaml::MappingTraits for the MultipartReply and
/// MultipartReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMULTIPARTREPLY_H_
#define OFP_YAML_YMULTIPARTREPLY_H_

#include "ofp/multipartreply.h"
#include "ofp/yaml/ympflowstatsreply.h"
#include "ofp/yaml/ympdesc.h"
#include "ofp/yaml/ympaggregatestatsreply.h"
#include "ofp/yaml/ymptablestats.h"
#include "ofp/yaml/ympportstats.h"
#include "ofp/yaml/ympqueuestats.h"

namespace ofp {
namespace detail {

/// Elements are fixed size.
template <class Type>
class MPReplyFixedSizeSeq {
 public:
  explicit MPReplyFixedSizeSeq(MultipartReply &msg)
      : msg_(msg), position_{msg.replyBody()} {}

  size_t size() const { return msg_.replyBodySize() / sizeof(Type); }

  Type &next() {
    const UInt8 *pos = position_;
    position_ += sizeof(Type);
    return RemoveConst_cast(*reinterpret_cast<const Type *>(pos));
  }

 private:
  MultipartReply &msg_;
  const UInt8 *position_;
};

/// Elements are variable size. First two bytes of an element contain the
/// element size.
template <class Type>
class MPReplyVariableSizeSeq {
 public:
  explicit MPReplyVariableSizeSeq(MultipartReply &msg)
      : msg_(msg), position_{msg.replyBody()} {}

  size_t size() const {
    size_t result = 0;
    size_t total = msg_.replyBodySize();
    const UInt8 *buf = msg_.replyBody();
    // FIXME check alignment
    size_t len = 0;
    while (len < total) {
      UInt16 elemSize = *reinterpret_cast<const Big16 *>(buf + len);
      len += elemSize;
      ++result;
    }
    return result;
  }

  Type &next() {
    const UInt8 *pos = position_;
    position_ += *reinterpret_cast<const Big16 *>(position_);
    return RemoveConst_cast(*reinterpret_cast<const Type *>(pos));
  }

 private:
  MultipartReply &msg_;
  const UInt8 *position_;
};

OFP_BEGIN_IGNORE_PADDING

template <class Type>
class MPReplyBuilderSeq {
 public:
  explicit MPReplyBuilderSeq(UInt8 version) : channel_{version}, init_{false} {}

  Type &next() {
    if (init_) {
      item_.write(&channel_);
      item_.reset();
    } else {
      init_ = true;
    }
    return item_;
  }

  void close() { (void)next(); }

  const UInt8 *data() const { return channel_.data(); }
  size_t size() const { return channel_.size(); }

 private:
  MemoryChannel channel_;
  Type item_;
  bool init_;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyFixedSizeSeq<Type>> {

  static size_t size(IO &io, ofp::detail::MPReplyFixedSizeSeq<Type> &seq) {
    return seq.size();
  }

  static Type &element(IO &io, ofp::detail::MPReplyFixedSizeSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyVariableSizeSeq<Type>> {

  static size_t size(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq) {
    return seq.size();
  }

  static Type &element(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyBuilderSeq<Type>> {

  static size_t size(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq) {
    return 0;
  }

  static Type &element(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

//---
// type: OFPT_MULTIPART_REPLY
// msg:
//   type: <OFPMultipartType>    { Required }
//   flags: <UInt16>             { Required }
//   body:
//       { Depends on OFPMultipartType }
//...

template <>
struct MappingTraits<ofp::MultipartReply> {

  static void mapping(IO &io, ofp::MultipartReply &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.replyType();
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.flags_);

    switch (type) {
      case OFPMP_DESC: {
        MPDesc *desc = RemoveConst_cast(msg.body_cast<MPDesc>());
        if (desc) {
          io.mapRequired("body", *desc);
        }
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowStatsReply> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReply *reply =
            RemoveConst_cast(msg.body_cast<MPAggregateStatsReply>());
        if (reply) {
          io.mapRequired("body", *reply);
        }
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyFixedSizeSeq<MPTableStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyFixedSizeSeq<MPPortStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyFixedSizeSeq<MPQueueStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_PORT_DESC: {
        // io.mapOptional("body", EmptyRequest);
        break;
      }
      default:
        // FIXME
        log::info("MultiPartReply: MappingTraits not fully implemented.",
                  int(type));
        break;
    }
  }
};

template <>
struct MappingTraits<ofp::MultipartReplyBuilder> {

  static void mapping(IO &io, ofp::MultipartReplyBuilder &msg) {
    using namespace ofp;

    OFPMultipartType type;
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.msg_.flags_);
    msg.setReplyType(type);

    switch (type) {
      case OFPMP_DESC: {
        MPDescBuilder desc;
        io.mapRequired("body", desc);
        // FIXME - write reply into channel.
        msg.setReplyBody(&desc, sizeof(desc));
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyBuilderSeq<MPFlowStatsReplyBuilder> seq{
            msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReplyBuilder reply;
        io.mapRequired("body", reply);
        // FIXME - write reply into channel.
        msg.setReplyBody(&reply, sizeof(reply));
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyBuilderSeq<MPTableStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPPortStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyBuilderSeq<MPQueueStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_DESC: {
        // io.mapOptional("body", EmptyRequest);
        break;
      }
      default:
        // FIXME
        log::info("MultiPartReplyBuilder: MappingTraits not fully implemented.",
                  int(type));
        break;
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMULTIPARTREPLY_H_
