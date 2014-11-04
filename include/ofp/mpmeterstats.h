// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPMETERSTATS_H_
#define OFP_MPMETERSTATS_H_

#include "ofp/packetcounterlist.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;

class MPMeterStats {
 public:
  enum { MPVariableSizeOffset = 4 };

  UInt32 meterId() const { return meterId_; }
  UInt32 flowCount() const { return flowCount_; }
  UInt64 packetInCount() const { return packetInCount_; }
  UInt64 byteInCount() const { return byteInCount_; }
  UInt32 durationSec() const { return durationSec_; }
  UInt32 durationNSec() const { return durationNSec_; }

  PacketCounterRange bandStats() const;

  bool validateInput(Validation *context) const;

 private:
  Big32 meterId_;
  Big16 len_;
  Padding<6> pad_;
  Big32 flowCount_;
  Big64 packetInCount_;
  Big64 byteInCount_;
  Big32 durationSec_;
  Big32 durationNSec_;

  friend class MPMeterStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPMeterStats) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPMeterStats>(), "Expected standard layout.");

class MPMeterStatsBuilder {
 public:
  MPMeterStatsBuilder() = default;

  void setMeterId(UInt32 meterId) { msg_.meterId_ = meterId; }
  void setFlowCount(UInt32 flowCount) { msg_.flowCount_ = flowCount; }
  void setPacketInCount(UInt64 packetInCount) {
    msg_.packetInCount_ = packetInCount;
  }
  void setByteInCount(UInt64 byteInCount) { msg_.byteInCount_ = byteInCount; }
  void setDurationSec(UInt32 durationSec) { msg_.durationSec_ = durationSec; }
  void setDurationNSec(UInt32 durationNSec) {
    msg_.durationNSec_ = durationNSec;
  }

  void write(Writable *channel);
  void reset() { bandStats_.clear(); }

 private:
  MPMeterStats msg_;
  PacketCounterList bandStats_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPMETERSTATS_H_
