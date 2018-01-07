// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacket.h"
#include "ofp/constants.h"
#include "ofp/macaddress.h"
#include "ofp/oxmfields.h"
#include "ofp/pkt.h"

using namespace ofp;

// OpenFlow specifies the alignment of the packet data. It is aligned so
// an IP, IPv6, ARP header portion is aligned to an 8-byte boundary.
// All decode functions in this class assume this alignment. However,
// if the input data is not aligned, we fix it up so it is -- this involves
// making a copy.

static bool isAlignedPacket(const UInt8 *ptr) {
  return (reinterpret_cast<uintptr_t>(ptr) & 0x07) == 2;
}

static const MacAddress *findLLOption(const UInt8 *ptr, size_t length,
                                      UInt8 option);

// The MatchPacket parser sets the following fields:
//
// Always:
//    ETH_DST
//    ETH_SRC
//    ETH_TYPE
//
// When ETH_TYPE == 0x8100:
//    VLAN_VID  
//    VLAN_PCP  (TODO if non-zero)
//    ETH_TYPE
//
// When ETH_TYPE == 0x0806:
//    ARP_OP    
//    ARP_SPA
//    ARP_TPA
//    ARP_SHA
//    ARP_THA
//
// When ETH_TYPE == 0x0800:
//    IP_DSCP  (TODO if non-zero)
//    IP_ECN   (TODO if non-zero)
//    IP_PROTO
//    IPV4_SRC
//    IPV4_DST
//    NX_IP_FRAG  (fragments only)
//    NX_IP_TTL
//
// When ETH_TYPE == 0x86dd:
//    IP_DSCP  (TODO if non-zero)
//    IP_ECN   (TODO if non-zero)
//    NX_IP_TTL
//    IPV6_SRC
//    IPV6_DST
//    IPV6_FLABEL  (TODO if non-zero)
//    IP_PROTO     (fixme: only if recognized?)
//    IPV6_EXTHDR
//
// When IP_PROTO == 0x06:
//    TCP_SRC
//    TCP_DST
//    NX_TCP_FLAGS
//
// When IP_PROTO == 0x11:
//    UDP_SRC
//    UDP_DST
//
// When ETH_TYPE == 0x0800 and IP_PROTO == 0x01:
//    ICMPV4_TYPE
//    ICMPV4_CODE
//
// When ETH_TYPE == 0x86dd and IP_PROTO == 0x3A:
//    ICMPV6_TYPE
//    ICMPV6_CODE
//    X_IPV6_ND_RES
//    IPV6_ND_TARGET
//    IPV6_ND_SLL
//    IPV6_ND_TLL
//
// When ETH_TYPE == 0x88cc or ETH_TYPE == 0x8942:
//    X_LLDP_CHASSIS_ID
//    X_LLDP_PORT_ID
//    X_LLDP_TTL

MatchPacket::MatchPacket(const ByteRange &data, bool warnMisaligned) {
  if (isAlignedPacket(data.data())) {
    decode(data.data(), data.size());
    return;
  }

  auto misalignment = reinterpret_cast<uintptr_t>(data.data()) & 0x07;
  assert(misalignment != 2 && misalignment < 8);

  if (warnMisaligned) {
    log_warning("MatchPacket: Misaligned input: ", misalignment);
  }

  // Make a copy of the packet that is appropriately aligned.
  size_t pad = (10 - misalignment) % 8;
  ByteList alignedData;
  alignedData.addZeros(pad);
  alignedData.add(data.data(), data.size());
  decode(alignedData.data() + pad, alignedData.size() - pad);
}

// Flag used internally to represent OFPIEH_DEST when it appears in an IPv6
// header chain just before a Router header.

const UInt32 OFPIEH_DEST_ROUTER = 1UL << 31;

void MatchPacket::decode(const UInt8 *pkt, size_t length) {
  assert(isAlignedPacket(pkt));

  decodeEthernet(pkt, length);

  // If all the data is not accounted for, report offset using X_PKT_POS.
  if (offset_ < length) {
    match_.addUnchecked(X_PKT_POS{UInt16_narrow_cast(offset_)});
  }
}

void MatchPacket::decodeEthernet(const UInt8 *pkt, size_t length) {
  assert(isAlignedPacket(pkt));

  auto eth = pkt::Ethernet::cast(pkt, length);
  if (!eth) {
    return;
  }

  match_.addUnchecked(OFB_ETH_DST(eth->dst));
  match_.addUnchecked(OFB_ETH_SRC(eth->src));

  pkt += sizeof(pkt::Ethernet);
  length -= sizeof(pkt::Ethernet);
  offset_ += sizeof(pkt::Ethernet);

  // Handle 802.1Q tag: ethType = 0x8100.
  UInt16 ethType = eth->type;
  if (ethType == DATALINK_8021Q) {
    auto vlan = pkt::VlanHdr::cast(pkt, length);
    if (!vlan) {
      // Missing complete vlan header?
      match_.addUnchecked(OFB_ETH_TYPE(ethType));
      return;
    }

    // N.B. Continue the OpenFlow tradition of setting the OFPVID_PRESENT bit.
    UInt16 vlan_vid = (vlan->tci & 0x0FFF) | OFPVID_PRESENT;
    match_.addUnchecked(OFB_VLAN_VID(vlan_vid));

    // (TODO) Include VLAN_PCP only if it's non-zero.
    UInt8 vlan_pcp = vlan->tci >> 13;
    match_.addUnchecked(OFB_VLAN_PCP(vlan_pcp));

    ethType = vlan->ethType;

    pkt += sizeof(pkt::VlanHdr);
    length -= sizeof(pkt::VlanHdr);
    offset_ += sizeof(pkt::VlanHdr);
  }

  match_.addUnchecked(OFB_ETH_TYPE(ethType));

  switch (ethType) {
    case DATALINK_ARP:
      decodeARP(pkt, length);
      break;

    case DATALINK_IPV4:
      decodeIPv4(pkt, length);
      break;

    case DATALINK_IPV6:
      decodeIPv6(pkt, length);
      break;

    case DATALINK_LLDP:
    case DATALINK_BDDP:
      decodeLLDP(pkt, length);
      break;

    default:
      log_debug("MatchPacket: Unknown ethernet type", eth->type);
      break;
  }
}

void MatchPacket::decodeARP(const UInt8 *pkt, size_t length) {
  auto arp = pkt::Arp::cast(pkt, length);
  if (!arp) {
    return;
  }

  if (std::memcmp(arp->prefix, OFP_ARP_PREFIX_STR, sizeof(arp->prefix)) != 0) {
    log_warning("MatchPacket: Unexpected arp prefix",
                RawDataToHex(arp->prefix, sizeof(arp->prefix)));
    return;
  }

  match_.addUnchecked(OFB_ARP_OP(arp->op));
  match_.addUnchecked(OFB_ARP_SPA(arp->spa));
  match_.addUnchecked(OFB_ARP_TPA(arp->tpa));
  match_.addUnchecked(OFB_ARP_SHA(arp->sha));
  match_.addUnchecked(OFB_ARP_THA(arp->tha));

  pkt += sizeof(pkt::Arp);
  length -= sizeof(pkt::Arp);
  offset_ += sizeof(pkt::Arp);

  if (IsMemFilled(pkt, length, 0)) {
    // Ignore padding added to ARP message only if it's all zeros.
    offset_ += length;
  }
}

void MatchPacket::decodeIPv4(const UInt8 *pkt, size_t length) {
  auto ip = pkt::IPv4Hdr::cast(pkt, length);
  if (!ip) {
    return;
  }

  UInt8 vers = ip->ver >> 4;
  UInt8 ihl = ip->ver & 0x0F;
  unsigned hdrLen = ihl * 4;

  if (vers != pkt::kIPv4Version) {
    log_warning("MatchPacket: Unexpected IPv4 version", vers);
    return;
  }

  if (hdrLen < sizeof(pkt::IPv4Hdr)) {
    log_warning("MatchPacket: IPv4 header too short", hdrLen);
    return;
  }

  if (hdrLen > length) {
    log_warning("MatchPacket: IPv4 header too long", hdrLen);
    return;
  }

  unsigned totalLen = ip->length;

  if (totalLen < hdrLen) {
    log_warning("MatchPacket: IPv4 total length < hdrLen", totalLen, hdrLen);
    return;
  }

  if (totalLen < length) {
    log_debug("MatchPacket: IPv4 total length < remaining length", totalLen,
              length);
    length = totalLen;
  }

  assert(length <= totalLen);

  UInt8 dscp = ip->tos >> 2;
  UInt8 ecn = ip->tos & 0x03;

  match_.addUnchecked(OFB_IP_DSCP{dscp});
  match_.addUnchecked(OFB_IP_ECN{ecn});
  match_.addUnchecked(OFB_IP_PROTO{ip->proto});
  match_.addUnchecked(OFB_IPV4_SRC{ip->src});
  match_.addUnchecked(OFB_IPV4_DST{ip->dst});

  UInt16 frag = ip->frag & 0x3fff;  // ignore DF bit
  UInt8 nxFragType = NXM_FRAG_TYPE_NONE;
  if (frag) {
    nxFragType = pkt::nxmFragmentType(frag);
    match_.addUnchecked(NXM_NX_IP_FRAG{nxFragType});
  }

  match_.addUnchecked(NXM_NX_IP_TTL{ip->ttl});

  pkt += hdrLen;
  length -= hdrLen;
  offset_ += hdrLen;

  // Stop decoding the packet here if the IPv4 packet is fragmented and this
  // is a "later" fragment.
  if (nxFragType & NXM_FRAG_TYPE_LATER) {
    return;
  }

  decodeIPv4_NextHdr(pkt, length, ip->proto);
}

void MatchPacket::decodeIPv4_NextHdr(const UInt8 *pkt, size_t length,
                                     UInt8 nextHdr) {
  switch (nextHdr) {
    case PROTOCOL_ICMP:
      decodeICMPv4(pkt, length);
      break;

    case PROTOCOL_TCP:
      decodeTCP(pkt, length);
      break;

    case PROTOCOL_UDP:
      decodeUDP(pkt, length);
      break;

    default:
      log_debug("MatchPacket: Unknown IPv4 protocol", nextHdr);
      break;
  }
}

void MatchPacket::decodeIPv6(const UInt8 *pkt, size_t length) {
  auto ip = pkt::IPv6Hdr::cast(pkt, length);
  if (!ip) {
    return;
  }

  UInt32 verClassLabel = ip->verClassLabel;
  UInt8 vers = verClassLabel >> 28;
  if (vers != pkt::kIPv6Version) {
    log_warning("MatchPacket: Unexpected IPv6 version", vers);
    return;
  }

  UInt8 trafCls = (verClassLabel >> 20) & 0x0FF;
  UInt32 flowLabel = verClassLabel & 0x000FFFFF;

  UInt8 dscp = trafCls >> 2;
  UInt8 ecn = trafCls & 0x03;

  match_.addUnchecked(OFB_IP_DSCP{dscp});
  match_.addUnchecked(OFB_IP_ECN{ecn});
  match_.addUnchecked(NXM_NX_IP_TTL{ip->hopLimit});
  match_.addUnchecked(OFB_IPV6_SRC{ip->src});
  match_.addUnchecked(OFB_IPV6_DST{ip->dst});
  match_.addUnchecked(OFB_IPV6_FLABEL{flowLabel});

  pkt += sizeof(pkt::IPv6Hdr);
  length -= sizeof(pkt::IPv6Hdr);
  offset_ += sizeof(pkt::IPv6Hdr);

  decodeIPv6_NextHdr(pkt, length, ip->nextHeader);
}

void MatchPacket::decodeTCP(const UInt8 *pkt, size_t length) {
  auto tcp = pkt::TCPHdr::cast(pkt, length);
  if (!tcp) {
    return;
  }

  UInt16 flags = tcp->flags & 0x0FFF;
  match_.addUnchecked(OFB_TCP_SRC{tcp->srcPort});
  match_.addUnchecked(OFB_TCP_DST{tcp->dstPort});
  match_.addUnchecked(NXM_NX_TCP_FLAGS{flags});

  offset_ += sizeof(pkt::TCPHdr);
}

void MatchPacket::decodeUDP(const UInt8 *pkt, size_t length) {
  auto udp = pkt::UDPHdr::cast(pkt, length);
  if (!udp) {
    return;
  }

  match_.addUnchecked(OFB_UDP_SRC{udp->srcPort});
  match_.addUnchecked(OFB_UDP_DST{udp->dstPort});

  offset_ += sizeof(pkt::UDPHdr);
}

void MatchPacket::decodeIPv6_NextHdr(const UInt8 *pkt, size_t length,
                                     UInt8 nextHdr) {
  UInt32 hdrFlags = 0;

  while (nextHdr != pkt::IPv6Ext_NoNextHeader) {
    switch (nextHdr) {
      case PROTOCOL_TCP:
        match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
        decodeTCP(pkt, length);
        nextHdr = pkt::IPv6Ext_NoNextHeader;
        break;

      case PROTOCOL_UDP:
        match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
        decodeUDP(pkt, length);
        nextHdr = pkt::IPv6Ext_NoNextHeader;
        break;

      case PROTOCOL_ICMPV6:
        match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
        decodeICMPv6(pkt, length);
        nextHdr = pkt::IPv6Ext_NoNextHeader;
        break;

      default:
        // Record that we saw this extension header, then advance to
        // the next.
        nextHdr = nextIPv6ExtHdr(nextHdr, pkt, length, hdrFlags);
        break;
    }
  }

  if (hdrFlags == 0) {
    hdrFlags |= OFPIEH_NONEXT;
  } else if (hdrFlags & OFPIEH_DEST_ROUTER) {
    hdrFlags = (hdrFlags & 0x0ffff) | OFPIEH_DEST;
  }

  match_.addUnchecked(
      OFB_IPV6_EXTHDR{static_cast<OFPIPv6ExtHdrFlags>(hdrFlags)});
}

void MatchPacket::decodeICMPv4(const UInt8 *pkt, size_t length) {
  auto icmp = pkt::ICMPHdr::cast(pkt, length);
  if (!icmp) {
    return;
  }

  match_.addUnchecked(OFB_ICMPV4_TYPE{icmp->type});
  match_.addUnchecked(OFB_ICMPV4_CODE{icmp->code});

  offset_ += sizeof(pkt::ICMPHdr);
}

void MatchPacket::decodeICMPv6(const UInt8 *pkt, size_t length) {
  auto icmp = pkt::ICMPHdr::cast(pkt, length);
  if (!icmp) {
    return;
  }

  match_.addUnchecked(OFB_ICMPV6_TYPE{icmp->type});
  match_.addUnchecked(OFB_ICMPV6_CODE{icmp->code});

  pkt += sizeof(pkt::ICMPHdr);
  length -= sizeof(pkt::ICMPHdr);
  offset_ += sizeof(pkt::ICMPHdr);

  if (icmp->code == 0 && (icmp->type == ICMPV6_TYPE_NEIGHBOR_SOLICIT ||
                          icmp->type == ICMPV6_TYPE_NEIGHBOR_ADVERTISE)) {
    decodeICMPv6_ND(pkt, length, icmp->type);
  }
}

void MatchPacket::decodeICMPv6_ND(const UInt8 *pkt, size_t length,
                                  UInt8 icmpv6Type) {
  assert(icmpv6Type == ICMPV6_TYPE_NEIGHBOR_SOLICIT ||
         icmpv6Type == ICMPV6_TYPE_NEIGHBOR_ADVERTISE);

  // RFC 2461: Neighbor Discovery (ND)
  // Format:
  //    [ 32 bits reserved | 128 bits address | variable options... ]

  if (length < 20) {
    log_warning("decodeICMPv6_ND: data too short");
    return;
  }

  if (icmpv6Type == ICMPV6_TYPE_NEIGHBOR_ADVERTISE) {
    // The reserved bits are only meaningful in ND Advertise.
    Big32 reserved = Big32_unaligned(pkt);
    match_.addUnchecked(X_IPV6_ND_RES{reserved});
  }

  const IPv6Address *addr = Interpret_cast<IPv6Address>(pkt + 4);
  match_.addUnchecked(OFB_IPV6_ND_TARGET{*addr});

  pkt += 20;
  length -= 20;
  offset_ += 20;

  if (length == 0) {  // No options
    return;
  }

  if (icmpv6Type == ICMPV6_TYPE_NEIGHBOR_SOLICIT) {
    const MacAddress *mac = findLLOption(pkt, length, ICMPV6_OPTION_SLL);
    if (mac) {
      match_.addUnchecked(OFB_IPV6_ND_SLL{*mac});
      offset_ += length;
    }
  } else if (icmpv6Type == ICMPV6_TYPE_NEIGHBOR_ADVERTISE) {
    const MacAddress *mac = findLLOption(pkt, length, ICMPV6_OPTION_TLL);
    if (mac) {
      match_.addUnchecked(OFB_IPV6_ND_TLL{*mac});
      offset_ += length;
    }
  }
}

void MatchPacket::decodeLLDP(const UInt8 *pkt, size_t length) {
  // LLDP packets are composed of TLV sections (tag-length-value).

  while (length > 0) {
    auto lldp = pkt::LLDPTlv::cast(pkt, length);
    if (!lldp) {
      log_warning("decodeLLDP: missing lldp end tlv");
      return;
    }

    auto jumpSize = lldp->length() + 2;
    if (jumpSize > length) {
      log_warning("decodeLLDP: malformed lldp tlv");
      return;  // ignore the rest
    }

    switch (lldp->type()) {
      case pkt::LLDPTlv::END:
        offset_ += jumpSize;
        return;  // all done; ignore anything else

      case pkt::LLDPTlv::CHASSIS_ID:
        match_.addUnchecked(X_LLDP_CHASSIS_ID{lldp->value()});
        break;

      case pkt::LLDPTlv::PORT_ID:
        match_.addUnchecked(X_LLDP_PORT_ID{lldp->value()});
        break;

      case pkt::LLDPTlv::TTL:
        match_.addUnchecked(X_LLDP_TTL{lldp->value16()});
        break;
    }

    assert(length >= jumpSize);

    pkt += jumpSize;
    length -= jumpSize;
    offset_ += jumpSize;
  }
}

UInt8 MatchPacket::nextIPv6ExtHdr(UInt8 currHdr, const UInt8 *&pkt,
                                  size_t &length, UInt32 &flags) {
  // RFC 2460 Section 4.1: Extension Header Order
  //
  //       Hop-by-Hop Options header
  //       Destination Options header (*)
  //       Routing header
  //       Fragment header
  //       Authentication header
  //       Encapsulating Security Payload header
  //       Destination Options header (*)
  //
  // Each extension header should occur at most once, except for the
  // Destination Options header which should occur at most twice (once
  // before a Routing header and once before the upper-layer header).

  assert(currHdr != pkt::IPv6Ext_NoNextHeader);

  auto ext = pkt::IPv6ExtHdr::cast(pkt, length);
  if (!ext) {
    return pkt::IPv6Ext_NoNextHeader;
  }

  // Masks for ordering extension headers by examining predecessors.
  const UInt32 kPreHopMask = 0;
  const UInt32 kPreDestRouterMask = kPreHopMask | OFPIEH_HOP;
  const UInt32 kPreRouterMask = kPreDestRouterMask | OFPIEH_DEST_ROUTER;
  const UInt32 kPreFragmentMask = kPreRouterMask | OFPIEH_ROUTER;
  const UInt32 kPreAuthMask = kPreFragmentMask | OFPIEH_FRAG;
  const UInt32 kPreESPMask = kPreAuthMask | OFPIEH_AUTH;
  const UInt32 kPreDestMask = kPreESPMask | OFPIEH_ESP;

  UInt8 nextHdr = ext->nextHeader;

  switch (currHdr) {
    case pkt::IPv6Ext_Hop:
      countIPv6ExtHdr(flags, OFPIEH_HOP, kPreHopMask);
      break;

    case pkt::IPv6Ext_Dest:
      if (nextHdr == pkt::IPv6Ext_Router) {
        countIPv6ExtHdr(flags, OFPIEH_DEST_ROUTER, kPreDestRouterMask);
      } else if (!pkt::IPv6NextHeaderIsExtension(nextHdr)) {
        countIPv6ExtHdr(flags, OFPIEH_DEST, kPreDestMask);
      } else {
        if (flags & OFPIEH_DEST)
          flags |= OFPIEH_UNREP;
        flags |= (OFPIEH_DEST | OFPIEH_UNSEQ);
      }
      break;

    case pkt::IPv6Ext_Router:
      countIPv6ExtHdr(flags, OFPIEH_ROUTER, kPreRouterMask);
      break;

    case pkt::IPv6Ext_Fragment:
      countIPv6ExtHdr(flags, OFPIEH_FRAG, kPreFragmentMask);
      break;

    case pkt::IPv6Ext_Auth:
      countIPv6ExtHdr(flags, OFPIEH_AUTH, kPreAuthMask);
      break;

    case pkt::IPv6Ext_ESP:
      countIPv6ExtHdr(flags, OFPIEH_ESP, kPreESPMask);
      break;

    case pkt::IPv6Ext_Mobility:
    case pkt::IPv6Ext_HostIdentity:
    case pkt::IPv6Ext_Shim6:
    case pkt::IPv6Ext_Experimental253:
    case pkt::IPv6Ext_Experimental254:
      // OpenFlow protocol ignores these extension headers.
      break;

    default:
      log_debug("MatchPacket: Unrecognized IPv6 nextHeader", currHdr);
      return pkt::IPv6Ext_NoNextHeader;
  }

  // Fragment header is always 8 bytes in size. Otherwise, we need to compute
  // header size from `hdrLen`.
  size_t extHdrLen = 8;
  if (currHdr != pkt::IPv6Ext_Fragment) {
    extHdrLen = (ext->hdrLen + 1) * 8;
  }

  if (extHdrLen > length) {
    log_warning("MatchPacket: IPv6 ext header extends past end of pkt");
    return pkt::IPv6Ext_NoNextHeader;
  }

  pkt += extHdrLen;
  length -= extHdrLen;
  offset_ += extHdrLen;

  return nextHdr;
}

void MatchPacket::countIPv6ExtHdr(UInt32 &flags, UInt32 hdr,
                                  UInt32 precedingHdrs) {
  if (flags & ~(hdr | precedingHdrs)) {
    flags |= OFPIEH_UNSEQ;
  }
  if (flags & hdr) {
    flags |= OFPIEH_UNREP;
  }
  flags |= hdr;
}

const MacAddress *findLLOption(const UInt8 *ptr, size_t length, UInt8 option) {
  // Option format:
  //   [ 8 bits type | 8 bits length | Variable option data... ]
  //
  // The length is in units of 8-octets, and includes the option header.
  // A zero-length is not allowed.
  //
  while (length >= 8) {
    // Check for invalid option length.
    const unsigned optionLen = ptr[1] * 8;

    if (optionLen == 0 || optionLen > length) {
      log_warning("ICMPv6 ND Option: invalid length detected");
      break;
    }

    // Is this the option we are looking for?
    if (ptr[0] == option && optionLen == 8) {
      return Interpret_cast<MacAddress>(ptr + 2);
    }

    ptr += optionLen;
    length -= optionLen;
  }

  return nullptr;
}
