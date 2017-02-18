// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PKT_H_
#define OFP_PKT_H_

#include "ofp/byteorder.h"
#include "ofp/byterange.h"
#include "ofp/constants.h"
#include "ofp/ipv6address.h"
#include "ofp/log.h"
#include "ofp/macaddress.h"

namespace ofp {
namespace pkt {

enum { kIPv4Version = 4, kIPv6Version = 6 };

template <class Type>
struct Castable {
  static const Type *cast(const void *ptr, size_t len) {
    assert(IsPtrAligned(ptr, alignof(Type)));
    if (len < sizeof(Type)) {
      // Only warn if there is some data.
      if (len > 0) {
        log_warning("pkt::Castable: Data too short, actualLen:", len,
                    "minNeeded:", sizeof(Type));
      } else {
        log_debug("pkt::Castable: No data. minNeeded:", sizeof(Type));
      }
      return nullptr;
    }
    return reinterpret_cast<const Type *>(ptr);
  }
};

struct Ethernet : public Castable<Ethernet> {
  MacAddress dst;
  MacAddress src;
  Big16 type;
};

static_assert(sizeof(Ethernet) == 14, "Unexpected size.");
static_assert(alignof(Ethernet) == 2, "Unexpected alignment.");

struct VlanHdr : public Castable<VlanHdr> {
  Big16 tci;      // pcp=3-bits dei=1-bit vid=12-bits
  Big16 ethType;  // actual ethernet type
};

static_assert(sizeof(VlanHdr) == 4, "Unexpected size.");
static_assert(alignof(VlanHdr) == 2, "Unexpected alignment.");

struct Arp : public Castable<Arp> {
  UInt8 prefix[6];  // fixed: ht, pt, hl, pl (see OFP_ARP_PREFIX_STR)
  Big16 op;
  MacAddress sha;
  IPv4Address spa;
  MacAddress tha;
  IPv4Address tpa;
};

#define OFP_ARP_PREFIX_STR "\x00\x01\x08\x00\x06\x04"

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

inline UInt8 nxmFragmentType(UInt16 frag) {
  UInt16 flags = frag & (IPv4_MoreFragMask | IPv4_FragOffsetMask);
  if (flags == IPv4_MoreFragMask)
    return NXM_FRAG_TYPE_ANY;
  else if (flags != 0)
    return NXM_FRAG_TYPE_ANY | NXM_FRAG_TYPE_LATER;
  return NXM_FRAG_TYPE_NONE;
}

struct IPv6Hdr : public Castable<IPv6Hdr> {
  Big32 verClassLabel;  // version: 4, trafficClass: 8, flowLabel: 20
  Big16 payloadLength;
  Big8 nextHeader;
  Big8 hopLimit;
  IPv6Address src;
  IPv6Address dst;
};

static_assert(sizeof(IPv6Hdr) == 40, "Unexpected size.");
static_assert(alignof(IPv6Hdr) == 4, "Unexpected alignment.");

enum : UInt8 {
  IPv6Ext_Hop = 0,
  IPv6Ext_Router = 43,
  IPv6Ext_Fragment = 44,  // ignore hdrLen
  IPv6Ext_ESP = 50,
  IPv6Ext_Auth = 51,
  IPv6Ext_NoNextHeader = 59,
  IPv6Ext_Dest = 60,
  IPv6Ext_Mobility = 135,
  IPv6Ext_HostIdentity = 139,
  IPv6Ext_Shim6 = 140,
  IPv6Ext_Experimental253 = 253,
  IPv6Ext_Experimental254 = 254
};

inline bool IPv6NextHeaderIsExtension(UInt8 nextHeader) {
  const UInt8 buf[] = {IPv6Ext_Hop,
                       IPv6Ext_Router,
                       IPv6Ext_Fragment,
                       IPv6Ext_ESP,
                       IPv6Ext_Auth,
                       IPv6Ext_Dest,
                       IPv6Ext_Mobility,
                       IPv6Ext_HostIdentity,
                       IPv6Ext_Shim6,
                       IPv6Ext_Experimental253,
                       IPv6Ext_Experimental254};
  return std::memchr(buf, nextHeader, ArrayLength(buf)) != nullptr;
}

// RFC 6564 - Standardized extension header format
// N.B. Fragment header is always 8 bytes long (ignore hdrLen)
struct IPv6ExtHdr : public Castable<IPv6ExtHdr> {
  Big8 nextHeader;
  Big8 hdrLen;
  Big8 data[6];
};

static_assert(sizeof(IPv6ExtHdr) == 8, "Unexpected size.");
static_assert(alignof(IPv6ExtHdr) == 1, "Unexpected alignment.");

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

struct LLDPTlv : public Castable<LLDPTlv> {
  Big8 taglen_[2];  // Combined tag:7, length:9

  explicit LLDPTlv(UInt8 type, size_t length) {
    assert(length < 512);
    taglen_[0] =
        UInt8_narrow_cast((UInt32_cast(type) << 1) | (length & 0x0100));
    taglen_[1] = (length & 0x00FF);
  }
  UInt8 type() const { return (taglen_[0] >> 1); }
  size_t length() const { return ((taglen_[0] & 0x01U) << 8U) | taglen_[1]; }
  const UInt8 *data() const { return BytePtr(this) + sizeof(taglen_); }

  ByteRange value() const { return ByteRange{data(), length()}; }
  Big32 value32() const { return Big32::fromBytes(data(), length()); }
  Big16 value16() const { return Big16::fromBytes(data(), length()); }

  enum {
    END = 0,
    CHASSIS_ID = 1,
    PORT_ID = 2,
    TTL = 3,
    PORT_DESCR = 4,
    SYS_NAME = 5,
    SYS_DESCR = 6,
    SYS_CAPABILITIES = 7,
    MGMT_ADDRESS = 8,
    ORG_SPECIFIC = 127
  };
};

static_assert(sizeof(LLDPTlv) == 2, "Unexpected size.");
static_assert(alignof(LLDPTlv) == 1, "Unexpected alignment.");

}  // namespace pkt
}  // namespace ofp

#endif  // OFP_PKT_H_
