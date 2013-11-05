#ifndef OFP_AGGREGATESTATSREPLY_H
#define OFP_AGGREGATESTATSREPLY_H

#include "ofp/padding.h"
#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

class MultipartReply;

class AggregateStatsReply {
public:
  static const AggregateStatsReply *cast(const MultipartReply *req);

  AggregateStatsReply() = default;

  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }
  UInt32 flowCount() const { return flowCount_; }

  bool validateLength(size_t length) const;
  
private:
  Big64 packetCount_;
  Big64 byteCount_;
  Big32 flowCount_;
  Padding<4> pad_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(AggregateStatsReply) == 24, "Unexpected size.");
static_assert(IsStandardLayout<AggregateStatsReply>(),
              "Expected standard layout.");

class AggregateStatsReplyBuilder {
public:


private:
  AggregateStatsReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_AGGREGATESTATSREPLY_H
