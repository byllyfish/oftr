#include "ofp/matchpacket.h"
#include "ofp/enetaddress.h"
#include "ofp/oxmfields.h"
#include "ofp/constants.h"

using namespace ofp;

// OpenFlow specifies the alignment of the packet data. It is aligned so
// an IP, IPv6, ARP header portion is aligned to an 8-byte boundary.
// All decode functions in this class assume this alignment. However,
// if the input data is not aligned, we fix it up so it is -- this involves
// making a copy.

static bool isAlignedPacket(const UInt8 *ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) & 0x07) == 2;
}


MatchPacket::MatchPacket(const ByteRange &data) {
    if (isAlignedPacket(data.data())) {
        decodeEthernet(data.data(), data.size());
        return;
    }

    // Make a copy of the packet that is appropriately aligned.
    log::warning("MatchPacket: Fixing misaligned input!");

    ByteList alignedData;
    alignedData.addZeros(2);
    alignedData.add(data.data(), data.size());
    decodeEthernet(alignedData.data(), alignedData.size());
}

namespace pkt {

enum {
    kIPv4Version = 4,
    kIPv6Version = 6,
};

template <class Type>
struct Castable {
    static const Type *const cast(const void *const ptr, size_t len) {
        assert(IsPtrAligned(ptr, alignof(Type)));
        if (len < sizeof(Type)) {
            log::warning("pkt::Castable: Data too short, actualLen:", len, "minNeeded:", sizeof(Type));
            return nullptr;
        }
        return reinterpret_cast<const Type *const>(ptr);
    }
};

struct Ethernet : public Castable<Ethernet> {
    EnetAddress dst;
    EnetAddress src;
    Big16 type;
};

static_assert(sizeof(Ethernet) == 14, "Unexpected size.");
static_assert(alignof(Ethernet) == 2, "Unexpected alignment.");

struct Arp : public Castable<Arp> {
    UInt8 prefix[6];        // fixed: ht, pt, hl, pl
    Big16 op;
    EnetAddress sha;
    IPv4Address spa;
    EnetAddress tha;
    IPv4Address tpa;
};

static_assert(sizeof(Arp) == 28, "Unexpected size.");
static_assert(alignof(Arp) == 2, "Unexpected alignment.");

struct IPv4Hdr : public Castable<IPv4Hdr> {
    Big8 ver;
    Big8 tos;
    Big16 length;
    Big16 ident;
    Big16 frag;
    Big8 ttl;
    Big8 proto;
    Big16 cksum;
    IPv4Address src;
    IPv4Address dst;
};

static_assert(sizeof(IPv4Hdr) == 20, "Unexpected size.");
static_assert(alignof(IPv4Hdr) == 2, "Unexpected alignment.");

enum : UInt16 {
    IPv4_MoreFragMask = 0x2000,
    IPv4_FragOffsetMask = 0x1FFF,
};

static UInt8 nxmFragmentType(UInt16 frag) {
    UInt16 flags = frag & (IPv4_MoreFragMask | IPv4_FragOffsetMask);
    if (flags == IPv4_MoreFragMask)
        return NXM_FRAG_TYPE_ANY;
    else if (flags != 0)
        return NXM_FRAG_TYPE_ANY | NXM_FRAG_TYPE_LATER;
    return NXM_FRAG_TYPE_NONE;
}

struct IPv6Hdr : public Castable<IPv6Hdr> {
    Big32 verClassLabel;       // version: 4, trafficClass: 8, flowLabel: 20
    Big16 payloadLength;
    Big8 nextHeader;
    Big8 hopLimit;
    IPv6Address src;
    IPv6Address dst;
};

static_assert(sizeof(IPv6Hdr) == 40, "Unexpected size.");
static_assert(alignof(IPv6Hdr) == 4, "Unexpected alignment.");

struct IPv6ExtHdr : public Castable<IPv6ExtHdr> {
    Big8 nextHeader;
    Big8 hdrLen;
    Big8 data[6];
};

static_assert(sizeof(IPv6ExtHdr) == 8, "Unexpected size.");
static_assert(alignof(IPv6ExtHdr) == 1, "Unexpected alignment.");

enum : UInt8 {
    IPv6_NoNextHeader = 59
};

struct ICMPHdr : public Castable<ICMPHdr> {
    Big8 type;
    Big8 code;
    Big16 cksum;
};

static_assert(sizeof(ICMPHdr) == 4, "Unexpected size.");
static_assert(alignof(ICMPHdr) == 2, "Unexpected alignment.");

struct TCPHdr : public Castable<TCPHdr> {
    Big16 srcPort;
    Big16 dstPort;
    Big32 seqNum;
    Big32 ackNum;
    Big16 flags;
    Big16 winSize;
    Big16 cksum;
    Big16 urgPtr;
};

static_assert(sizeof(TCPHdr) == 20, "Unexpected size.");
static_assert(alignof(TCPHdr) == 4, "Unexpected alignment.");

struct UDPHdr : public Castable<UDPHdr> {
    Big16 srcPort;
    Big16 dstPort;
    Big16 length;
    Big16 cksum;
};

static_assert(sizeof(UDPHdr) == 8, "Unexpected size.");
static_assert(alignof(UDPHdr) == 2, "Unexpected alignment.");

}  // namespace pkt


void MatchPacket::decodeEthernet(const UInt8 *pkt, size_t length) {
    assert(isAlignedPacket(pkt));

    auto eth = pkt::Ethernet::cast(pkt, length);
    if (!eth) {
        return;
    }

    match_.add(OFB_ETH_DST(eth->dst));
    match_.add(OFB_ETH_SRC(eth->src));
    match_.add(OFB_ETH_TYPE(eth->type));

    pkt += sizeof(pkt::Ethernet);
    length -= sizeof(pkt::Ethernet);

    switch (eth->type) {
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
            decodeLLDP(pkt, length);
            break;

        default:
            log::warning("MatchPacket: Unknown ethernet type", eth->type);
            break;
    }
}

void MatchPacket::decodeARP(const UInt8 *pkt, size_t length) {
    auto arp = pkt::Arp::cast(pkt, length);
    if (!arp) {
        return;
    }

    if (std::memcmp(arp->prefix, "\x00\x01\x08\x00\x06\x04", sizeof(arp->prefix)) != 0) {
        log::warning("MatchPacket: Unexpected arp prefix", log::hex(arp->prefix));
        return;
    }

    match_.add(OFB_ARP_OP(arp->op));
    match_.add(OFB_ARP_SPA(arp->spa));
    match_.add(OFB_ARP_TPA(arp->tpa));
    match_.add(OFB_ARP_SHA(arp->sha));
    match_.add(OFB_ARP_THA(arp->tha));

    if (length > sizeof(pkt::Arp)) {
        log::warning("MatchPacket: Ignoring extra data in arp message");
    }
}
    

void MatchPacket::decodeIPv4(const UInt8 *pkt, size_t length) {
    auto ip = pkt::IPv4Hdr::cast(pkt, length);
    if (!ip) {
        return;
    }

    UInt8 vers = ip->ver >> 4;
    UInt8 ihl = ip->ver & 0x0F;

    if (vers != pkt::kIPv4Version) {
        log::warning("MatchPacket: Unexpected IPv4 version", vers);
        return;
    }

    if (ihl < 5) {
        log::warning("MatchPacket: Unexpected IPv4 header length", ihl);
        return;
    }

    UInt8 dscp = ip->tos >> 2;
    UInt8 ecn = ip->tos & 0x03;

    match_.add(OFB_IP_DSCP{dscp});
    match_.add(OFB_IP_ECN{ecn});
    match_.add(OFB_IP_PROTO{ip->proto});
    match_.add(OFB_IPV4_SRC{ip->src});
    match_.add(OFB_IPV4_DST{ip->dst});

    if (ip->frag) {
        match_.add(NXM_NX_IP_FRAG{pkt::nxmFragmentType(ip->frag)});
    }
    
    match_.add(NXM_NX_IP_TTL{ip->ttl});

    unsigned hdrLen = ihl * 4;
    assert(hdrLen <= 60);

    if (hdrLen > length) {
        log::warning("MatchPacket: Extended IPv4 Header too long", hdrLen);
        return;
    }

    pkt += hdrLen;
    length -= hdrLen;

    decodeIPv4_NextHdr(pkt, length, ip->proto);
}

void MatchPacket::decodeIPv4_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr) {
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
            log::warning("MatchPacket: Unknown IPv4 protocol", nextHdr);
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
        log::warning("MatchPacket: Unexpected IPv6 version", vers);
        return;
    }

    UInt8 trafCls = (verClassLabel >> 20) & 0x0FF;
    UInt32 flowLabel = verClassLabel & 0x000FFFFF;

    UInt8 dscp = trafCls >> 2;
    UInt8 ecn = trafCls & 0x03;

    match_.add(OFB_IP_DSCP{dscp});
    match_.add(OFB_IP_ECN{ecn});
    match_.add(OFB_IPV6_SRC{ip->src});
    match_.add(OFB_IPV6_DST{ip->src});
    match_.add(OFB_IPV6_FLABEL{flowLabel});

    pkt += sizeof(pkt::IPv6Hdr);
    length -= sizeof(pkt::IPv6Hdr);

    decodeIPv6_NextHdr(pkt, length, ip->nextHeader);
}

void MatchPacket::decodeTCP(const UInt8 *pkt, size_t length) {
    auto tcp = pkt::TCPHdr::cast(pkt, length);
    if (!tcp) {
        return;
    }

    UInt16 flags = tcp->flags & 0x0FFF;
    match_.add(OFB_TCP_SRC{tcp->srcPort});
    match_.add(OFB_TCP_DST{tcp->dstPort});
    match_.add(NXM_NX_TCP_FLAGS{flags});
}

void MatchPacket::decodeUDP(const UInt8 *pkt, size_t length) {
    auto udp = pkt::UDPHdr::cast(pkt, length);
    if (!udp) {
        return;
    }

    match_.add(OFB_UDP_SRC{udp->srcPort});
    match_.add(OFB_UDP_DST{udp->dstPort});
}


void MatchPacket::decodeIPv6_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr) {

    while (nextHdr != pkt::IPv6_NoNextHeader) {

        switch (nextHdr) {
            case PROTOCOL_TCP:
                match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
                decodeTCP(pkt, length);
                nextHdr = pkt::IPv6_NoNextHeader;
                break;

            case PROTOCOL_UDP:
                match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
                decodeUDP(pkt, length);
                nextHdr = pkt::IPv6_NoNextHeader;
                break;

            case PROTOCOL_ICMPV6:
                match_.addOrderedUnchecked(OFB_IP_PROTO{nextHdr});
                decodeICMPv6(pkt, length);
                nextHdr = pkt::IPv6_NoNextHeader;
                break;

            default:
                // Record that we saw this extension header, then advance to
                // the next.
                nextHdr = nextIPv6ExtHdr(pkt, length);
                break;
        }
    }
}

void MatchPacket::decodeICMPv4(const UInt8 *pkt, size_t length) {
    auto icmp = pkt::ICMPHdr::cast(pkt, length);
    if (!icmp) {
        return;
    }

    match_.add(OFB_ICMPV4_TYPE{icmp->type});
    match_.add(OFB_ICMPV4_CODE{icmp->code});
}

void MatchPacket::decodeICMPv6(const UInt8 *pkt, size_t length) {
    auto icmp = pkt::ICMPHdr::cast(pkt, length);
    if (!icmp) {
        return;
    }

    match_.add(OFB_ICMPV6_TYPE{icmp->type});
    match_.add(OFB_ICMPV6_CODE{icmp->code});

    // TODO(bfish): neighbor discovery msgs.
}

void MatchPacket::decodeLLDP(const UInt8 *pkt, size_t length) {

}

UInt8 MatchPacket::nextIPv6ExtHdr(const UInt8 *&pkt, size_t &length) {
    assert(IsPtrAligned(pkt, 8));

    auto ext = pkt::IPv6ExtHdr::cast(pkt, length);
    if (!ext) {
        return pkt::IPv6_NoNextHeader;
    }

    size_t extHdrLen = (ext->hdrLen + 1) * 8;
    if (extHdrLen > length) {
        log::warning("MatchPacket: IPv6 ext header extends past end of pkt");
        return pkt::IPv6_NoNextHeader;
    }

    pkt += extHdrLen;
    length -= extHdrLen;

    return ext->nextHeader;
}
