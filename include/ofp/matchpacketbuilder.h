// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MATCHPACKETBUILDER_H_
#define OFP_MATCHPACKETBUILDER_H_

#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/lldpvalue.h"
#include "ofp/macaddress.h"
#include "ofp/oxmrange.h"
#include "ofp/vlannumber.h"

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
  UInt8 icmpType_ = 0;
  UInt8 icmpCode_ = 0;
  IPv4Address ipv4Src_;
  IPv4Address ipv4Dst_;
  IPv6Address ipv6Src_;
  IPv6Address ipv6Dst_;
  UInt16 srcPort_ = 0;
  UInt16 dstPort_ = 0;
  UInt8 ipProto_ = 0;
  UInt8 ipTtl_ = 0;
  IPv6Address ndTarget_;
  MacAddress ndLl_;
  UInt32 ndRes_ = 0;
  LLDPValue<LLDPType::ChassisID> lldpChassisId_;
  LLDPValue<LLDPType::PortID> lldpPortId_;
  UInt16 lldpTtl_ = 0;
  LLDPValue<LLDPType::SysName> lldpSysName_;
  LLDPValue<LLDPType::PortDescr> lldpPortDescr_;
  LLDPValue<LLDPType::Custom> lldpCustom_;

  void addEthernet(ByteList *msg) const;
  void addIPv4(ByteList *msg, size_t length) const;
  void addIPv6(ByteList *msg, size_t length) const;

  void buildEthernet(ByteList *msg, const ByteRange &data) const;
  void buildArp(ByteList *msg) const;
  void buildLldp(ByteList *msg) const;
  void buildIPv4(ByteList *msg, const ByteRange &data) const;
  void buildIPv4_other(ByteList *msg, const ByteRange &data) const;
  void buildICMPv4(ByteList *msg, const ByteRange &data) const;
  void buildTCPv4(ByteList *msg, const ByteRange &data) const;
  void buildIPv6(ByteList *msg, const ByteRange &data) const;
  void buildIPv6_other(ByteList *msg, const ByteRange &data) const;
  void buildICMPv6(ByteList *msg, const ByteRange &data) const;
  void buildICMPv6_ND(ByteList *msg) const;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_MATCHPACKETBUILDER_H_
