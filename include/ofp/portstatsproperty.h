// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_PORTSTATSPROPERTY_H_
#define OFP_PORTSTATSPROPERTY_H_

#include "ofp/experimenterproperty.h"

namespace ofp {

class PortStatsPropertyEthernet : private NonCopyable {
 public:
  constexpr static OFPPortStatsProperty type() { return OFPPSPT_ETHERNET; }

  void setRxFrameErr(UInt64 rxFrameErr) { rxFrameErr_ = rxFrameErr; }
  void setRxOverErr(UInt64 rxOverErr) { rxOverErr_ = rxOverErr; }
  void setRxCrcErr(UInt64 rxCrcErr) { rxCrcErr_ = rxCrcErr; }
  void setCollisions(UInt64 collisions) { collisions_ = collisions; }

 private:
  Big16 type_ = type();
  Big16 len_ = 40;
  Padding<4> pad_;
  Big64 rxFrameErr_;
  Big64 rxOverErr_;
  Big64 rxCrcErr_;
  Big64 collisions_;

  friend class MPPortStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortStatsPropertyEthernet) == 40, "Unexpected size.");

// PortStatsPropertyOptical needs to be `Copyable` since it is used as an
// optional property.

class PortStatsPropertyOptical {
 public:
  constexpr static OFPPortStatsProperty type() { return OFPPSPT_OPTICAL; }

 private:
  Big16 type_ = type();
  Big16 len_ = 44;
  Padding<4> pad_;
  Big32 flags_;
  Big32 txFreqLmda_;
  Big32 txOffset_;
  Big32 txGridSpan_;
  Big32 rxFreqLmda_;
  Big32 rxOffset_;
  Big32 rxGridSpan_;
  Big16 txPwr_;
  Big16 rxPwr_;
  Big16 biasCurrent_;
  Big16 temperature_;
  Padding<4> pad2_;  // This padding is not part of the property.

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortStatsPropertyOptical) == 48, "Unexpected size.");

using PortStatsPropertyExperimenter =
    detail::ExperimenterProperty<OFPPortStatsProperty, OFPPSPT_EXPERIMENTER>;

}  // namespace ofp

#endif  // OFP_PORTSTATSPROPERTY_H_
