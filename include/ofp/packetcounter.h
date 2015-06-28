// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PACKETCOUNTER_H_
#define OFP_PACKETCOUNTER_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

class PacketCounter {
 public:
  enum {
    ProtocolIteratorSizeOffset = PROTOCOL_ITERATOR_SIZE_FIXED,
    ProtocolIteratorAlignment = 8
  };

  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }

  bool validateInput(Validation *context) const { return true; }
  
 private:
  Big64 packetCount_;
  Big64 byteCount_;

  friend class PacketCounterBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PacketCounter) == 16, "Unexpected size.");
static_assert(IsStandardLayout<PacketCounter>(), "Expected standard layout.");

class PacketCounterBuilder {
 public:
  PacketCounterBuilder() = default;

  void setPacketCount(UInt64 packetCount) { msg_.packetCount_ = packetCount; }
  void setByteCount(UInt64 byteCount) { msg_.byteCount_ = byteCount; }

 private:
  PacketCounter msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PACKETCOUNTER_H_
