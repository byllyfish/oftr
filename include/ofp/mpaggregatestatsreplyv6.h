#ifndef OFP_MPAGGREGATESTATSREPLYV6_H_
#define OFP_MPAGGREGATESTATSREPLYV6_H_

#include "ofp/statbuilder.h"

namespace ofp {

class MultipartReply;
class Writable;

class MPAggregateStatsReplyV6 {
 public:
  static const MPAggregateStatsReplyV6 *cast(const MultipartReply *req);

  MPAggregateStatsReplyV6() = default;

  UInt64 packetCount() const;
  UInt64 byteCount() const;
  UInt32 flowCount() const;

  Stat stat() const;
  
  bool validateInput(Validation *context) const;

 private:
  StatHeader stats_;

  const StatHeader *statHeader() const;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPAggregateStatsReplyV6) == 4, "Unexpected size.");
static_assert(IsStandardLayout<MPAggregateStatsReplyV6>(),
              "Expected standard layout.");

class MPAggregateStatsReplyV6Builder {
 public:
  void setPacketCount(UInt64 packetCount);
  void setByteCount(UInt64 byteCount);
  void setFlowCount(UInt32 flowCount);

  void write(Writable *channel);

 private:
  StatBuilder stats_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPAGGREGATESTATSREPLYV6_H_
