//  ===== ---- ofp/mpportstats.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the MPPortStats and MPPortStatsBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPPORTSTATS_H_
#define OFP_MPPORTSTATS_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class Validation;

class MPPortStats {
 public:
  MPPortStats() = default;

  UInt32 portNo() const { return portNo_; }
  UInt64 rxPackets() const { return rxPackets_; }

  bool validateInput(Validation *context) const { return true; }

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
  void reset() {}

 private:
  MPPortStats msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPPORTSTATS_H_
