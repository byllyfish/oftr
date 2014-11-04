// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPAGGREGATESTATSREPLY_H_
#define OFP_MPAGGREGATESTATSREPLY_H_

#include "ofp/padding.h"
#include "ofp/byteorder.h"

namespace ofp {

class MultipartReply;
class Validation;

class MPAggregateStatsReply {
 public:
  static const MPAggregateStatsReply *cast(const MultipartReply *req);

  MPAggregateStatsReply() = default;

  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }
  UInt32 flowCount() const { return flowCount_; }

  bool validateInput(Validation *context) const;

 private:
  Big64 packetCount_;
  Big64 byteCount_;
  Big32 flowCount_;
  Padding<4> pad_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPAggregateStatsReply) == 24, "Unexpected size.");
static_assert(IsStandardLayout<MPAggregateStatsReply>(),
              "Expected standard layout.");

class MPAggregateStatsReplyBuilder {
 public:
 private:
  MPAggregateStatsReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPAGGREGATESTATSREPLY_H_
