// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacketbuilder.h"
#include "ofp/oxmfields.h"
#include "ofp/pkt.h"

using namespace ofp;

#define FieldBit(oxm_) (1UL << (oxm_::type().oxmField() - 1))

const UInt64 kRequiredEthernet =
    FieldBit(OFB_ETH_DST) | FieldBit(OFB_ETH_SRC) | FieldBit(OFB_ETH_TYPE);

const UInt64 kRequiredArp = kRequiredEthernet | FieldBit(OFB_ARP_OP) |
                            FieldBit(OFB_ARP_SHA) | FieldBit(OFB_ARP_SPA) |
                            FieldBit(OFB_ARP_THA) | FieldBit(OFB_ARP_TPA);

const UInt64 kRequiredLLDP = kRequiredEthernet | FieldBit(X_LLDP_CHASSIS_ID) |
                             FieldBit(X_LLDP_PORT_ID) | FieldBit(X_LLDP_TTL);

MatchPacketBuilder::MatchPacketBuilder(const OXMRange &range) {
  for (const auto &item : range) {
    OXMType type = item.type();

    // Track which fields are present, so we can detect missing fields.
    present_ |= (1UL << (type.oxmField() - 1));

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
#if 0
        case DATALINK_IPv4:
            buildIPv4(msg, data);
            break;
#endif  // 0
  }
}

void MatchPacketBuilder::buildEthernet(ByteList *msg) const {
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

void MatchPacketBuilder::buildArp(ByteList *msg) const {
  if ((present_ & kRequiredArp) != kRequiredArp) {
    reportMissingFields();
    return;
  }

  buildEthernet(msg);

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
  if ((present_ & kRequiredLLDP) != kRequiredLLDP) {
    reportMissingFields();
    return;
  }

  buildEthernet(msg);

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

#if 0

void MatchPacketBuilder::buildIPv4(ByteList *msg, const ByteRange &data) const {
    buildEthernet(msg);
    
    pkt::IPv4Hdr ip;
    ip->ver = 0x45;
    ip->tos = 0;
    ip->length = 0;
    ip->ident = 0;
    ip->frag = 0;
    ip->ttl = 0;
    ip->proto = ipProto_;
    ip->cksum = 0;
    ip->src = ipv4Src_;
    ip->dst = ipv4Dst_;

    switch (ipProto_) {
        case IPPROTO_ICMPv4:
            buildICMPv4(msg, data);
            break;
    }
}


void MatchPacketBuilder::buildICMPv4(ByteList *msg, const ByteRange &data) const {
    buildEthernet(msg);
    buildIPv4(msg);

    pkt::ICMPHdr icmp;
    icmp.type = 0;
    icmp.code = 0;
    icmp.cksum = 0;

    msg->add(data.data(), data.size());
}

#endif  // 0

void MatchPacketBuilder::reportMissingFields() const {
  log_info("MatchPacketBuilder: Missing Fields!", log::hex(present_),
           log::hex(kRequiredArp));
}
