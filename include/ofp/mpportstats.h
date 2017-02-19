// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPPORTSTATS_H_
#define OFP_MPPORTSTATS_H_

#include "ofp/byteorder.h"
#include "ofp/durationsec.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;
class Validation;

class MPPortStats {
 public:
  enum { MPVariableSizeOffset = 0 };

  MPPortStats() = default;

  PortNumber portNo() const { return portNo_; }
  DurationSec duration() const { return duration_; }
  UInt64 rxPackets() const { return rxPackets_; }
  UInt64 txPackets() const { return txPackets_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Padding<2> pad_;
  PortNumber portNo_;
  DurationSec duration_;
  Big64 rxPackets_;
  Big64 txPackets_;
  Big64 rxBytes_;
  Big64 txBytes_;
  Big64 rxDropped_;
  Big64 txDropped_;
  Big64 rxErrors_;
  Big64 txErrors_;

  friend class MPPortStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPPortStats) == 80, "Unexpected size.");
static_assert(IsStandardLayout<MPPortStats>(), "Expected standard layout.");

class MPPortStatsBuilder {
 public:
  MPPortStatsBuilder() = default;

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setDuration(DurationSec duration) { msg_.duration_ = duration; }
  void setRxPackets(UInt64 rxPackets) { msg_.rxPackets_ = rxPackets; }
  void setTxPackets(UInt64 txPackets) { msg_.txPackets_ = txPackets; }
  void setRxErrors(UInt64 rxErrors) { msg_.rxErrors_ = rxErrors; }
  void setTxErrors(UInt64 txErrors) { msg_.txErrors_ = txErrors; }

  void setProperties(const PropertyList &properties) {
    properties_ = properties;
    updateLen();
  }

  void write(Writable *channel);
  void reset() {}

 private:
  MPPortStats msg_;
  PropertyList properties_;

  enum { SizeWithoutProperties = sizeof(msg_) };

  void updateLen() {
    msg_.length_ =
        UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
  }

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPPORTSTATS_H_
