// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPTABLESTATS_H_
#define OFP_MPTABLESTATS_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/strings.h"

namespace ofp {

class Writable;
class Validation;

// MPTableStats supports v1.0 and v1.3 only.

class MPTableStats {
 public:
  MPTableStats() = default;

  UInt8 tableId() const { return tableId_; }
  std::string name() const { return name_.toString(); }
  UInt32 wildcards() const { return wildcards_; }
  UInt32 maxEntries() const { return maxEntries_; }
  UInt32 activeCount() const { return activeCount_; }
  UInt64 lookupCount() const { return lookupCount_; }
  UInt64 matchedCount() const { return matchedCount_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Big8 tableId_;
  Padding<3> pad_;
  TableNameStr name_;
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
  void reset() {}

 private:
  MPTableStats msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPTABLESTATS_H_
