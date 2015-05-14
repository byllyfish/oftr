// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPPORTSTATS_H_
#define OFP_MPPORTSTATS_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;
class Validation;

class MPPortStats {
 public:
  MPPortStats() = default;

  PortNumber portNo() const { return portNo_; }
  UInt64 rxPackets() const { return rxPackets_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  PortNumber portNo_;
  Padding<4> pad_;
  Big64 rxPackets_;
  Big64 txPackets_;
  Big64 rxBytes_;
  Big64 txBytes_;
  Big64 rxDropped_;
  Big64 txDropped_;
  Big64 rxErrors_;
  Big64 txErrors_;
  Big64 rxFrameErr_;
  Big64 rxOverErr_;
  Big64 rxCrcErr_;
  Big64 collisions_;
  Big32 durationSec_;
  Big32 durationNSec_;

  friend class MPPortStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPPortStats) == 112, "Unexpected size.");
static_assert(IsStandardLayout<MPPortStats>(), "Expected standard layout.");

class MPPortStatsBuilder {
 public:
  MPPortStatsBuilder() = default;

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setRxPackets(UInt64 rxPackets) { msg_.rxPackets_ = rxPackets; }
  void setTxPackets(UInt64 txPackets) { msg_.txPackets_ = txPackets; }
  void setDurationSec(UInt32 durationSec) { msg_.durationSec_ = durationSec; }
  void setDurationNSec(UInt32 durationNSec) { msg_.durationNSec_ = durationNSec; }
  void setRxErrors(UInt64 rxErrors) { msg_.rxErrors_ = rxErrors; }
  void setTxErrors(UInt64 txErrors) { msg_.txErrors_ = txErrors; }
  void setRxFrameErr(UInt64 rxFrameErr) { msg_.rxFrameErr_ = rxFrameErr; }
  void setRxOverErr(UInt64 rxOverErr) { msg_.rxOverErr_ = rxOverErr; }
  void setRxCrcErr(UInt64 rxCrcErr) { msg_.rxCrcErr_ = rxCrcErr; }
  void setCollisions(UInt64 collisions) { msg_.collisions_ = collisions; }

  void setProperties(const PropertyList &properties) { properties_ = properties; }

  void write(Writable *channel);
  void reset() {}

 private:
  MPPortStats msg_;
  PropertyList properties_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPPORTSTATS_H_
