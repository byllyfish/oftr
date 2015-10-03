// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ORIGINALMATCH_H_
#define OFP_ORIGINALMATCH_H_

#include "ofp/byteorder.h"
#include "ofp/macaddress.h"
#include "ofp/ipv4address.h"
#include "ofp/padding.h"
#include "ofp/oxmrange.h"

namespace ofp {
namespace deprecated {

struct OriginalMatch {
  enum Wildcards : UInt32 {
    OFPFW_IN_PORT = 1 << 0,   ///< Switch input port.
    OFPFW_DL_VLAN = 1 << 1,   ///< VLAN id.
    OFPFW_DL_SRC = 1 << 2,    ///< Ethernet source address.
    OFPFW_DL_DST = 1 << 3,    ///< Ethernet destination address.
    OFPFW_DL_TYPE = 1 << 4,   ///< Ethernet frame type.
    OFPFW_NW_PROTO = 1 << 5,  ///< IP protocol.
    OFPFW_TP_SRC = 1 << 6,    ///< TCP/UDP source port.
    OFPFW_TP_DST = 1 << 7,    ///< TCP/UDP destination port.

    // IP source address wildcard bit count. 0 is exact match, 1 ignores the
    // LSB, 2 ignores the 2 least-significant bits, ..., 32 and higher
    // wildcard the entire field. This is the *opposite* of the usual
    // convention where e.g. /24 indicates that 8 bits (not 24 bits) are
    // wildcarded.
    OFPFW_NW_SRC_SHIFT = 8,
    OFPFW_NW_SRC_BITS = 6,
    OFPFW_NW_SRC_MASK = ((1 << OFPFW_NW_SRC_BITS) - 1) << OFPFW_NW_SRC_SHIFT,
    OFPFW_NW_SRC_ALL = 32 << OFPFW_NW_SRC_SHIFT,

    // IP destination address wildcard bit count. Same format as source.
    OFPFW_NW_DST_SHIFT = 14,
    OFPFW_NW_DST_BITS = 6,
    OFPFW_NW_DST_MASK = ((1 << OFPFW_NW_DST_BITS) - 1) << OFPFW_NW_DST_SHIFT,
    OFPFW_NW_DST_ALL = 32 << OFPFW_NW_DST_SHIFT,
    OFPFW_DL_VLAN_PCP = 1 << 20,  ///< VLAN priority.
    OFPFW_NW_TOS = 1 << 21,       ///< IP ToS (DSCP field, 6 bits).

    // All fields wildcarded, but using OFPFW_NW_SRC_ALL and OFPFW_NW_DST_ALL.
    // This replaces the definition from the spec:
    //   OFPFW_ALL = ((1 << 22) - 1)
    OFPFW_ALL = 0x003820FF
  };

  OriginalMatch() = default;
  explicit OriginalMatch(const OXMRange &range);

  Big32 wildcards;
  Big16 in_port;
  MacAddress dl_src;
  MacAddress dl_dst;
  Big16 dl_vlan;
  Big8 dl_vlan_pcp;
  Padding<1> pad1;
  Big16 dl_type;
  Big8 nw_tos;
  Big8 nw_proto;
  Padding<2> pad2;
  IPv4Address nw_src;
  IPv4Address nw_dst;
  Big16 tp_src;
  Big16 tp_dst;

  IPv4Address nw_src_mask() const {
    unsigned bits = (wildcards & OFPFW_NW_SRC_MASK) >> OFPFW_NW_SRC_SHIFT;
    if (bits > 32)
      bits = 32;
    return IPv4Address::mask(32U - bits);
  }

  IPv4Address nw_dst_mask() const {
    unsigned bits = (wildcards & OFPFW_NW_DST_MASK) >> OFPFW_NW_DST_SHIFT;
    if (bits > 32)
      bits = 32;
    return IPv4Address::mask(32U - bits);
  }

  UInt32 standardWildcards() const;

  std::string toString() const;
};

static_assert(sizeof(OriginalMatch) == 40, "Unexpected size.");

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_ORIGINALMATCH_H_
