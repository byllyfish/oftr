// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPQUEUESTATS_H_
#define OFP_MPQUEUESTATS_H_

#include "ofp/byteorder.h"
#include "ofp/portnumber.h"
#include "ofp/queuenumber.h"
#include "ofp/durationsec.h"

namespace ofp {

class Writable;
class Validation;

class MPQueueStats {
 public:
  MPQueueStats() = default;

  bool validateInput(Validation *context) const { return true; }

 private:
  PortNumber portNo_;
  QueueNumber queueId_;
  Big64 txBytes_;
  Big64 txPackets_;
  Big64 txErrors_;
  DurationSec duration_;

  friend class MPQueueStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPQueueStats) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPQueueStats>(), "Expected standard layout.");

class MPQueueStatsBuilder {
 public:
  void write(Writable *channel);
  void reset() {}

 private:
  MPQueueStats msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPQUEUESTATS_H_
