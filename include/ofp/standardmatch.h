// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_STANDARDMATCH_H_
#define OFP_STANDARDMATCH_H_

#include "ofp/oxmfields.h"
#include "ofp/enetaddress.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/oxmlist.h"
#include "ofp/padding.h"
#include "ofp/log.h"
#include "ofp/constants.h"

namespace ofp {
namespace deprecated {

struct OriginalMatch;

enum { OFPMT_STANDARD_LENGTH = 88 };

struct StandardMatch {
  enum Wildcards : UInt32 {
    OFPFW_IN_PORT = 1 << 0,      ///< Switch input port
    OFPFW_DL_VLAN = 1 << 1,      ///< VLAN id
    OFPFW_DL_VLAN_PCP = 1 << 2,  ///< VLAN priority
    OFPFW_DL_TYPE = 1 << 3,      ///< Ethernet frame type
    OFPFW_NW_TOS = 1 << 4,       ///< IP ToS (DSCP field, 6 bits)
    OFPFW_NW_PROTO = 1 << 5,     ///< IP protocol
    OFPFW_TP_SRC = 1 << 6,       ///< TCP/UDP/SCTP source port
    OFPFW_TP_DST = 1 << 7,       ///< TCP/UDP/SCTP destination port
    OFPFW_MPLS_LABEL = 1 << 8,   ///< MPLS label
    OFPFW_MPLS_TC = 1 << 9,      ///< MPLS TC
    OFPFW_ALL = ((1 << 10) - 1)  ///< Wildcard all fields
  };

  Big16 type = OFPMT_STANDARD;
  Big16 length = OFPMT_STANDARD_LENGTH;
  Big32 in_port;
  Big32 wildcards = OFPFW_ALL;
  EnetAddress dl_src;
  EnetAddress dl_src_mask;
  EnetAddress dl_dst;
  EnetAddress dl_dst_mask;
  Big16 dl_vlan;
  Big8 dl_vlan_pcp;
  Padding<1> pad1_;
  Big16 dl_type;
  Big8 nw_tos;
  Big8 nw_proto;
  IPv4Address nw_src;
  IPv4Address nw_src_mask;
  IPv4Address nw_dst;
  IPv4Address nw_dst_mask;
  Big16 tp_src;
  Big16 tp_dst;
  Big32 mpls_label;
  Big8 mpls_tc;
  Padding<3> pad2_;
  Big64 metadata;
  Big64 metadata_mask;

  StandardMatch();
  explicit StandardMatch(const OXMRange &range);
  explicit StandardMatch(const OriginalMatch &match);

  OXMList toOXMList() const;

 private:
  void convertDatalinkARP(UInt32 wc, OXMList *list) const;
};

static_assert(sizeof(StandardMatch) == OFPMT_STANDARD_LENGTH,
              "Unexpected size.");

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_STANDARDMATCH_H_
