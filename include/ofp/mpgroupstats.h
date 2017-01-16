// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPGROUPSTATS_H_
#define OFP_MPGROUPSTATS_H_

#include "ofp/durationsec.h"
#include "ofp/groupnumber.h"
#include "ofp/packetcounterlist.h"

namespace ofp {

class Writable;

class MPGroupStats {
 public:
  enum { MPVariableSizeOffset = 0 };

  GroupNumber groupId() const { return groupId_; }
  UInt32 refCount() const { return refCount_; }
  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }
  DurationSec duration() const { return duration_; }

  PacketCounterRange bucketStats() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Padding<2> pad1_;
  GroupNumber groupId_;
  Big32 refCount_;
  Padding<4> pad2_;
  Big64 packetCount_;
  Big64 byteCount_;
  DurationSec duration_;

  friend class MPGroupStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupStats) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupStats>(), "Expected standard layout.");

class MPGroupStatsBuilder {
 public:
  MPGroupStatsBuilder() = default;

  void write(Writable *channel);
  void reset() { bucketStats_.clear(); }

 private:
  MPGroupStats msg_;
  PacketCounterList bucketStats_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPGROUPSTATS_H_
