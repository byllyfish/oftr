// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacketbuilder.h"
#include "ofp/oxmfields.h"
#include "ofp/pkt.h"

using namespace ofp;

const UInt8 kDefaultIPv4_TTL = 64;
const UInt8 kDefaultIPv6_TTL = 64;

// Ethernet frames have a minimum size of 64 bytes. Since the FCS is 4 bytes,
// this means we should pad out Ethernet packets to 60 bytes.
const size_t kMinEthernetFrameDataSize = 60;

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
        vlanVid_ = item.value<OFB_VLAN_VID>().value();
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
      case OFB_IPV6_SRC::type():
        ipv6Src_ = item.value<OFB_IPV6_SRC>();
        break;
      case OFB_IPV6_DST::type():
        ipv6Dst_ = item.value<OFB_IPV6_DST>();
        break;
      case OFB_IP_PROTO::type():
        ipProto_ = item.value<OFB_IP_PROTO>();
        break;
      case NXM_NX_IP_TTL::type():
        ipTtl_ = item.value<NXM_NX_IP_TTL>();
        break;
      case OFB_TCP_SRC::type():
        srcPort_ = item.value<OFB_TCP_SRC>();
        break;
      case OFB_TCP_DST::type():
        dstPort_ = item.value<OFB_TCP_DST>();
        break;
      case OFB_ICMPV4_CODE::type():
        icmpCode_ = item.value<OFB_ICMPV4_CODE>();
        break;
      case OFB_ICMPV6_CODE::type():
        icmpCode_ = item.value<OFB_ICMPV6_CODE>();
        break;
      case OFB_ICMPV4_TYPE::type():
        icmpType_ = item.value<OFB_ICMPV4_TYPE>();
        break;
      case OFB_ICMPV6_TYPE::type():
        icmpType_ = item.value<OFB_ICMPV6_TYPE>();
        break;
      case OFB_IPV6_ND_TARGET::type():
        ndTarget_ = item.value<OFB_IPV6_ND_TARGET>();
        break;
      case OFB_IPV6_ND_SLL::type():
        ndLl_ = item.value<OFB_IPV6_ND_SLL>();
        break;
      case OFB_IPV6_ND_TLL::type():
        ndLl_ = item.value<OFB_IPV6_ND_TLL>();
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
      case X_LLDP_SYS_NAME::type():
        lldpSysName_ = item.value<X_LLDP_SYS_NAME>();
        break;
      case X_LLDP_PORT_DESCR::type():
        lldpPortDescr_ = item.value<X_LLDP_PORT_DESCR>();
        break;
      case X_LLDP_ORG_SPECIFIC::type():
        lldpCustom_.push_back(item.value<X_LLDP_ORG_SPECIFIC>());
        break;
      case X_IPV6_ND_RES::type():
        ndRes_ = item.value<X_IPV6_ND_RES>();
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

    case DATALINK_IPV6:
      buildIPv6(msg, data);
      break;

    default:
      log_error("MatchPacketBuilder: Unknown ethType:", ethType_);
      buildEthernet(msg, data);
      break;
  }

  // If packet is less than 60 bytes, pad it with zeros.
  if (msg->size() < kMinEthernetFrameDataSize) {
    msg->addZeros(kMinEthernetFrameDataSize - msg->size());
  }
}

void MatchPacketBuilder::addEthernet(ByteList *msg) const {
  pkt::Ethernet eth;
  eth.dst = ethDst_;
  eth.src = ethSrc_;

  UInt16 vlanVid = vlanVid_;
  if (vlanVid == OFPVID_NONE) {
    eth.type = ethType_;
    msg->add(&eth, sizeof(eth));

  } else {
    pkt::VlanHdr vlanHdr;
    vlanHdr.tci = UInt16_narrow_cast((vlanPcp_ << 13) | (vlanVid & 0x0fff));
    vlanHdr.ethType = ethType_;

    eth.type = DATALINK_8021Q;
    msg->add(&eth, sizeof(eth));
    msg->add(&vlanHdr, sizeof(vlanHdr));
  }
}

void MatchPacketBuilder::addIPv4(ByteList *msg, size_t length) const {
  pkt::IPv4Hdr ip;

  std::memset(&ip, 0, sizeof(ip));
  ip.ver = 0x45;
  ip.length = UInt16_narrow_cast(sizeof(pkt::IPv4Hdr) + length);
  ip.ttl = ipTtl_ ? ipTtl_ : kDefaultIPv4_TTL;
  ip.proto = ipProto_;
  ip.src = ipv4Src_;
  ip.dst = ipv4Dst_;
  ip.cksum = pkt::Checksum({&ip, sizeof(ip)});

  msg->add(&ip, sizeof(ip));
}

void MatchPacketBuilder::addIPv6(ByteList *msg, size_t length) const {
  pkt::IPv6Hdr ip;

  std::memset(&ip, 0, sizeof(ip));
  ip.verClassLabel = 0x60000000;
  ip.payloadLength = UInt16_narrow_cast(length);
  ip.nextHeader = ipProto_;
  ip.hopLimit = ipTtl_ ? ipTtl_ : kDefaultIPv6_TTL;
  ip.src = ipv6Src_;
  ip.dst = ipv6Dst_;

  msg->add(&ip, sizeof(ip));
}

void MatchPacketBuilder::buildEthernet(ByteList *msg,
                                       const ByteRange &data) const {
  addEthernet(msg);
  msg->add(data.data(), data.size());
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

  if (!lldpSysName_.empty()) {
    pkt::LLDPTlv tlv{pkt::LLDPTlv::SYS_NAME, lldpSysName_.size()};
    msg->add(&tlv, sizeof(tlv));
    msg->add(lldpSysName_.data(), lldpSysName_.size());
  }

  if (!lldpPortDescr_.empty()) {
    pkt::LLDPTlv tlv{pkt::LLDPTlv::PORT_DESCR, lldpPortDescr_.size()};
    msg->add(&tlv, sizeof(tlv));
    msg->add(lldpPortDescr_.data(), lldpPortDescr_.size());
  }

  for (auto &customValue : lldpCustom_) {
    pkt::LLDPTlv tlv{pkt::LLDPTlv::ORG_SPECIFIC, customValue.size()};
    msg->add(&tlv, sizeof(tlv));
    msg->add(customValue.data(), customValue.size());
  }

  pkt::LLDPTlv end{pkt::LLDPTlv::END, 0};
  msg->add(&end, sizeof(end));
}

void MatchPacketBuilder::buildIPv4(ByteList *msg, const ByteRange &data) const {
  switch (ipProto_) {
    case PROTOCOL_ICMP:
      buildICMPv4(msg, data);
      break;

    case PROTOCOL_TCP:
      buildTCPv4(msg, data);
      break;

    default:
      log_error("MatchPacketBuilder: Unknown IPv4 protocol:", ipProto_);
      buildIPv4_other(msg, data);
      break;
  }
}

void MatchPacketBuilder::buildIPv4_other(ByteList *msg,
                                         const ByteRange &data) const {
  addEthernet(msg);
  addIPv4(msg, data.size());

  msg->add(data.data(), data.size());
}

void MatchPacketBuilder::buildICMPv4(ByteList *msg,
                                     const ByteRange &data) const {
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

void MatchPacketBuilder::buildTCPv4(ByteList *msg,
                                    const ByteRange &data) const {
  addEthernet(msg);
  addIPv4(msg, sizeof(pkt::TCPHdr) + data.size());

  pkt::TCPHdr tcp;
  std::memset(&tcp, 0, sizeof(tcp));
  tcp.srcPort = srcPort_;
  tcp.dstPort = dstPort_;
  tcp.cksum = pkt::Checksum({&tcp, sizeof(tcp)}, data);

  msg->add(&tcp, sizeof(tcp));
  msg->add(data.data(), data.size());
}

void MatchPacketBuilder::buildIPv6(ByteList *msg, const ByteRange &data) const {
  switch (ipProto_) {
    case PROTOCOL_ICMPV6:
      if (icmpType_ == ICMPV6_TYPE_NEIGHBOR_SOLICIT ||
          icmpType_ == ICMPV6_TYPE_NEIGHBOR_ADVERTISE) {
        buildICMPv6_ND(msg);
      } else {
        buildICMPv6(msg, data);
      }
      break;

    default:
      log_error("MatchPacketBuilder: Unknown IPv6 protocol:", ipProto_);
      buildIPv6_other(msg, data);
      break;
  }
}

void MatchPacketBuilder::buildIPv6_other(ByteList *msg,
                                         const ByteRange &data) const {
  addEthernet(msg);
  addIPv6(msg, data.size());

  msg->add(data.data(), data.size());
}

void MatchPacketBuilder::buildICMPv6(ByteList *msg,
                                     const ByteRange &data) const {
  const size_t len = sizeof(pkt::ICMPHdr) + data.size();

  addEthernet(msg);
  addIPv6(msg, len);

  pkt::IPv6PseudoHdr pseudoHdr;
  pseudoHdr.src = ipv6Src_;
  pseudoHdr.dst = ipv6Dst_;
  pseudoHdr.upperLength = UInt32_narrow_cast(len);
  pseudoHdr.nextHeader = ipProto_;

  pkt::ICMPHdr icmp;
  std::memset(&icmp, 0, sizeof(icmp));
  icmp.type = icmpType_;
  icmp.code = icmpCode_;
  icmp.cksum = pkt::Checksum({&pseudoHdr, sizeof(pseudoHdr)},
                             {&icmp, sizeof(icmp)}, data);

  msg->add(&icmp, sizeof(icmp));
  msg->add(data.data(), data.size());
}

void MatchPacketBuilder::buildICMPv6_ND(ByteList *msg) const {
  assert(icmpType_ == ICMPV6_TYPE_NEIGHBOR_SOLICIT ||
         icmpType_ == ICMPV6_TYPE_NEIGHBOR_ADVERTISE);

  Big8 hdr[2];
  hdr[0] = (icmpType_ == ICMPV6_TYPE_NEIGHBOR_SOLICIT) ? ICMPV6_OPTION_SLL
                                                       : ICMPV6_OPTION_TLL;
  hdr[1] = 1;  // length in 8-octet units

  ByteList buf;
  Big32 ndRes = (icmpType_ == ICMPV6_TYPE_NEIGHBOR_ADVERTISE) ? ndRes_ : 0;
  buf.add(&ndRes, sizeof(ndRes));
  buf.add(&ndTarget_, sizeof(ndTarget_));
  buf.add(&hdr[0], sizeof(hdr));
  buf.add(&ndLl_, sizeof(ndLl_));

  buildICMPv6(msg, buf.toRange());
}
