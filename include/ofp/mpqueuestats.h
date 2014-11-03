//  ===== ---- ofp/mpqueuestats.h --------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the MPQueueStats and MPQueueStatsBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPQUEUESTATS_H_
#define OFP_MPQUEUESTATS_H_

#include "ofp/byteorder.h"

namespace ofp {

class Writable;
class Validation;

class MPQueueStats {
 public:
  MPQueueStats() = default;

  bool validateInput(Validation *context) const { return true; }

 private:
  Big32 portNo_;
  Big32 queueId_;
  Big64 txBytes_;
  Big64 txPackets_;
  Big64 txErrors_;
  Big32 durationSec_;
  Big32 durationNSec_;

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
