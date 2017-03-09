// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacketbuilder.h"
#include "ofp/oxmfields.h"
#include "ofp/pkt.h"

using namespace ofp;


MatchPacketBuilder::MatchPacketBuilder(const OXMRange &range) {
  assert(range.validateInput());

  for (const auto &item : range) {
    OXMType type = item.type();

    switch (type) {
      case OFB_ETH_DST::type():
        ethDst_ = item.value<OFB_ETH_DST>();
        break;
      case OFB_ETH_SRC::type():
        ethSrc_ = item.value<OFB_ETH_SRC>();
        break;
      case OFB_ETH_TYPE::type():
        ethType_ = item.value<OFB_ETH_TYPE>();
        break;
      case OFB_VLAN_VID::type():
        vlanVid_ = item.value<OFB_VLAN_VID>();
        break;
      case OFB_VLAN_PCP::type():
        vlanPcp_ = item.value<OFB_VLAN_PCP>();
        break;
      case OFB_ARP_OP::type():
        arpOp_ = item.value<OFB_ARP_OP>();
        break;
      case OFB_ARP_SHA::type():
        arpSha_ = item.value<OFB_ARP_SHA>();
        break;
      case OFB_ARP_SPA::type():
        arpSpa_ = item.value<OFB_ARP_SPA>();
        break;
      case OFB_ARP_THA::type():
        arpTha_ = item.value<OFB_ARP_THA>();
        break;
      case OFB_ARP_TPA::type():
        arpTpa_ = item.value<OFB_ARP_TPA>();
        break;
      case OFB_IPV4_SRC::type():
        ipv4Src_ = item.value<OFB_IPV4_SRC>();
        break;
      case OFB_IPV4_DST::type():
        ipv4Dst_ = item.value<OFB_IPV4_DST>();
        break;
      case OFB_IP_PROTO::type():
        ipProto_ = item.value<OFB_IP_PROTO>();
        break;
      case OFB_ICMPV4_CODE::type():
        icmpCode_ = item.value<OFB_ICMPV4_CODE>();
        break;
      case OFB_ICMPV4_TYPE::type():
        icmpType_ = item.value<OFB_ICMPV4_TYPE>();
        break;
      case X_LLDP_CHASSIS_ID::type():
        lldpChassisId_ = item.value<X_LLDP_CHASSIS_ID>();
        break;
      case X_LLDP_PORT_ID::type():
        lldpPortId_ = item.value<X_LLDP_PORT_ID>();
        break;
      case X_LLDP_TTL::type():
        lldpTtl_ = item.value<X_LLDP_TTL>();
        break;
      default:
        log_warning("MatchPacketBuilder: Unknown field", type);
        break;
    }
  }
}

void MatchPacketBuilder::build(ByteList *msg, const ByteRange &data) const {
  assert(msg->size() == 0);

  switch (ethType_) {
    case DATALINK_ARP:
      buildArp(msg);
      break;

    case DATALINK_LLDP:
      buildLldp(msg);
      break;
        
    case DATALINK_IPV4:
      buildIPv4(msg, data);
      break;

    default:
      log_error("MatchPacketBuilder: Unknown ethType:", ethType_);
      break;
  }
}

void MatchPacketBuilder::addEthernet(ByteList *msg) const {
  pkt::Ethernet eth;
  eth.dst = ethDst_;
  eth.src = ethSrc_;

  if (vlanVid_ == OFPVID_NONE) {
    eth.type = ethType_;
    msg->add(&eth, sizeof(eth));

  } else {
    pkt::VlanHdr vlan;
    vlan.tci = UInt16_narrow_cast((vlanPcp_ << 13) | (vlanVid_ & 0x0fff));
    vlan.ethType = ethType_;

    eth.type = DATALINK_8021Q;
    msg->add(&eth, sizeof(eth));
    msg->add(&vlan, sizeof(vlan));
  }
}

void MatchPacketBuilder::addIPv4(ByteList *msg, size_t length) const {
    pkt::IPv4Hdr ip;

    std::memset(&ip, 0, sizeof(ip));
    ip.ver = 0x45;
    ip.length = UInt16_narrow_cast(sizeof(pkt::IPv4Hdr) + length);
    ip.ttl = 64;
    ip.proto = ipProto_;
    ip.src = ipv4Src_;
    ip.dst = ipv4Dst_;
    ip.cksum = pkt::Checksum({&ip, sizeof(ip)});

    msg->add(&ip, sizeof(ip));
}

void MatchPacketBuilder::buildArp(ByteList *msg) const {
  addEthernet(msg);

  pkt::Arp arp;
  std::memcpy(&arp.prefix, OFP_ARP_PREFIX_STR, sizeof(arp.prefix));
  arp.op = arpOp_;
  arp.sha = arpSha_;
  arp.spa = arpSpa_;
  arp.tha = arpTha_;
  arp.tpa = arpTpa_;

  msg->add(&arp, sizeof(arp));
}

void MatchPacketBuilder::buildLldp(ByteList *msg) const {
  addEthernet(msg);

  pkt::LLDPTlv tlv1{pkt::LLDPTlv::CHASSIS_ID, lldpChassisId_.size()};
  msg->add(&tlv1, sizeof(tlv1));
  msg->add(lldpChassisId_.data(), lldpChassisId_.size());

  pkt::LLDPTlv tlv2{pkt::LLDPTlv::PORT_ID, lldpPortId_.size()};
  msg->add(&tlv2, sizeof(tlv2));
  msg->add(lldpPortId_.data(), lldpPortId_.size());

  Big16 ttl = lldpTtl_;
  pkt::LLDPTlv tlv3{pkt::LLDPTlv::TTL, sizeof(ttl)};
  msg->add(&tlv3, sizeof(tlv3));
  msg->add(&ttl, sizeof(ttl));
}

void MatchPacketBuilder::buildIPv4(ByteList *msg, const ByteRange &data) const {
  switch (ipProto_) {
    case PROTOCOL_ICMP:
      buildICMPv4(msg, data);
      break;

    default:
      log_error("MatchPacketBuilder: Unknown IPv4 protocol:", ipProto_);
      break;
  }
}

void MatchPacketBuilder::buildICMPv4(ByteList *msg, const ByteRange &data) const {
    addEthernet(msg);
    addIPv4(msg, sizeof(pkt::ICMPHdr) + data.size());

    pkt::ICMPHdr icmp;
    std::memset(&icmp, 0, sizeof(icmp));
    icmp.type = icmpType_;
    icmp.code = icmpCode_;
    icmp.cksum = pkt::Checksum({&icmp, sizeof(icmp)}, data);

    msg->add(&icmp, sizeof(icmp));
    msg->add(data.data(), data.size());
}

