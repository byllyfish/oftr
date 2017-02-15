// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MATCHPACKETBUILDER_H_
#define OFP_MATCHPACKETBUILDER_H_

#include "ofp/ipv4address.h"
#include "ofp/lldpvalue.h"
#include "ofp/macaddress.h"
#include "ofp/oxmrange.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class MatchPacketBuilder {
 public:
  explicit MatchPacketBuilder(const OXMRange &range);

  void build(ByteList *msg, const ByteRange &data) const;

 private:
  OXMRange range_;
  MacAddress ethDst_;
  MacAddress ethSrc_;
  UInt16 ethType_ = 0;
  UInt16 vlanVid_ = 0;
  UInt8 vlanPcp_ = 0;
  UInt16 arpOp_ = 0;
  IPv4Address arpSpa_;
  IPv4Address arpTpa_;
  MacAddress arpSha_;
  MacAddress arpTha_;
  LLDPValue<LLDPType::ChassisID> lldpChassisId_;
  LLDPValue<LLDPType::PortID> lldpPortId_;
  UInt16 lldpTtl_ = 0;

  void buildEthernet(ByteList *msg) const;
  void buildArp(ByteList *msg) const;
  void buildLldp(ByteList *msg) const;

#if 0
    void buildIPv4(ByteList *msg, const ByteRange &data) const;
    void buildICMPv4(ByteList *msg, const ByteRange &data) const;
#endif  // 0
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_MATCHPACKETBUILDER_H_
