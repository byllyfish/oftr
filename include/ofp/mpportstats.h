#ifndef OFP_PORTSTATS_H
#define OFP_PORTSTATS_H

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class Writable;

class MPPortStats {
public:
  MPPortStats() = default;

  UInt32 portNo() const { return portNo_; }
  UInt64 rxPackets() const { return rxPackets_; }

private:
  Big32 portNo_;
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

  void write(Writable *channel);

private:
  MPPortStats msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_PORTSTATS_H
