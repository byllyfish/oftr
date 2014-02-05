//  ===== ---- ofp/mptablestats.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the MPTableStats and MPTableStatsBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPTABLESTATS_H_
#define OFP_MPTABLESTATS_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/smallcstring.h"

namespace ofp {

class Writable;

// MPTableStats supports v1.0 and v1.3 only.

class MPTableStats {
public:
  using TableStr = SmallCString<32>;

  MPTableStats() = default;

  UInt8 tableId() const { return tableId_; }
  std::string name() const { return name_.toString(); }
  UInt32 wildcards() const { return wildcards_; }
  UInt32 maxEntries() const { return maxEntries_; }
  UInt32 activeCount() const { return activeCount_; }
  UInt64 lookupCount() const { return lookupCount_; }
  UInt64 matchedCount() const { return matchedCount_; }

private:
  Big8 tableId_;
  Padding<3> pad_;
  TableStr name_;
  Big32 wildcards_;
  Big32 maxEntries_;
  Big32 activeCount_;
  Big64 lookupCount_;
  Big64 matchedCount_;

  friend class MPTableStatsBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPTableStats) == 64, "Unexpected size.");
static_assert(IsStandardLayout<MPTableStats>(), "Expected standard layout.");

class MPTableStatsBuilder {
public:
  MPTableStatsBuilder() = default;

  void write(Writable *channel);

private:
  MPTableStats msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPTABLESTATS_H_
