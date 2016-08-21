// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/originalmatch.h"
#include <iterator>
#include "ofp/standardmatch.h"

using namespace ofp;
using namespace ofp::deprecated;

using OW = OriginalMatch::Wildcards;
using SW = StandardMatch::Wildcards;

static void set_nw_src_mask(UInt32 &wc, unsigned prefix) {
  if (prefix > 32)
    prefix = 32;

  wc = (wc & ~OW::OFPFW_NW_SRC_MASK) |
       ((32U - prefix) << OW::OFPFW_NW_SRC_SHIFT);
}

static void set_nw_dst_mask(UInt32 &wc, unsigned prefix) {
  if (prefix > 32)
    prefix = 32;

  wc = (wc & ~OW::OFPFW_NW_DST_MASK) |
       ((32U - prefix) << OW::OFPFW_NW_DST_SHIFT);
}

OriginalMatch::OriginalMatch(const OXMRange &range) {
  std::memset(this, 0, sizeof(OriginalMatch));

  UInt32 wc = OFPFW_ALL;
  for (auto &item : range) {
    switch (item.type()) {
      case OFB_IN_PORT::type():
        in_port = UInt16_narrow_cast(item.value<OFB_IN_PORT>().value());
        wc &= ~OFPFW_IN_PORT;
        break;
      case OFB_ETH_SRC::type():
        dl_src = item.value<OFB_ETH_SRC>();
        wc &= ~OFPFW_DL_SRC;
        break;
      case OFB_ETH_DST::type():
        dl_dst = item.value<OFB_ETH_DST>();
        wc &= ~OFPFW_DL_DST;
        break;
      case OFB_ETH_TYPE::type():
        dl_type = item.value<OFB_ETH_TYPE>();
        wc &= ~OFPFW_DL_TYPE;
        break;
      case OFB_VLAN_VID::type(): {
        UInt16 vlan = item.value<OFB_VLAN_VID>();
        dl_vlan =
            (vlan == OFPVID_NONE ? v1::OFPVID_NONE : (vlan & ~OFPVID_PRESENT));
        wc &= ~OFPFW_DL_VLAN;
        break;
      }
      case OFB_VLAN_VID::typeWithMask(): {
        UInt16 vlan = item.value<OFB_VLAN_VID>();
        UInt16 mask = item.mask<OFB_VLAN_VID>();
        if (vlan == OFPVID_PRESENT && mask == OFPVID_PRESENT) {
          // This is a OpenFlow 1.1 feature; I didn't see it mentioned in the
          // 1.0 specification. Allow it anyway.
          dl_vlan = v1::OFPVID_PRESENT;
        } else {
          if (mask != 0xffff) {
            log_warning(
                "OriginalMatch: VLAN mask not supported. Reverting to specific "
                "match");
          }
          dl_vlan = (vlan & ~OFPVID_PRESENT);
        }
        wc &= ~OFPFW_DL_VLAN;
        break;
      }
      case OFB_VLAN_PCP::type():
        dl_vlan_pcp = item.value<OFB_VLAN_PCP>();
        wc &= ~OFPFW_DL_VLAN_PCP;
        break;
      case OFB_IP_DSCP::type():
        nw_tos = item.value<OFB_IP_DSCP>();
        wc &= ~OFPFW_NW_TOS;
        break;
      case OFB_IP_PROTO::type():
        nw_proto = item.value<OFB_IP_PROTO>();
        wc &= ~OFPFW_NW_PROTO;
        break;
      case OFB_IPV4_SRC::type():
        nw_src = item.value<OFB_IPV4_SRC>();
        set_nw_src_mask(wc, 32);
        break;
      case OFB_IPV4_SRC::type().withMask():
        nw_src = item.value<OFB_IPV4_SRC>();
        set_nw_src_mask(wc, item.mask<OFB_IPV4_SRC>().value().prefix());
        break;
      case OFB_IPV4_DST::type():
        nw_dst = item.value<OFB_IPV4_DST>();
        set_nw_dst_mask(wc, 32);
        break;
      case OFB_IPV4_DST::type().withMask():
        nw_dst = item.value<OFB_IPV4_DST>();
        set_nw_dst_mask(wc, item.mask<OFB_IPV4_DST>().value().prefix());
        break;
      case OFB_TCP_SRC::type():
        tp_src = item.value<OFB_TCP_SRC>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_TCP_DST::type():
        tp_dst = item.value<OFB_TCP_DST>();
        wc &= ~OFPFW_TP_DST;
        break;
      case OFB_UDP_SRC::type():
        tp_src = item.value<OFB_UDP_SRC>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_UDP_DST::type():
        tp_dst = item.value<OFB_UDP_DST>();
        wc &= ~OFPFW_TP_DST;
        break;
      case OFB_ICMPV4_TYPE::type():
        tp_src = item.value<OFB_ICMPV4_TYPE>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_ICMPV4_CODE::type():
        tp_dst = item.value<OFB_ICMPV4_CODE>();
        wc &= ~OFPFW_TP_DST;
        break;
      default:
        log_info("OriginalMatch: Unrecognized oxm type.", item.type());
        break;
    }
  }

  wildcards = wc;
}

struct WildcardInfo {
  OW original;
  SW standard;
};

static const WildcardInfo WildcardMap[] = {
    {OW::OFPFW_IN_PORT, SW::OFPFW_IN_PORT},
    {OW::OFPFW_DL_VLAN, SW::OFPFW_DL_VLAN},
    {OW::OFPFW_DL_TYPE, SW::OFPFW_DL_TYPE},
    {OW::OFPFW_NW_PROTO, SW::OFPFW_NW_PROTO},
    {OW::OFPFW_TP_SRC, SW::OFPFW_TP_SRC},
    {OW::OFPFW_TP_DST, SW::OFPFW_TP_DST},
    {OW::OFPFW_DL_VLAN_PCP, SW::OFPFW_DL_VLAN_PCP},
    {OW::OFPFW_NW_TOS, SW::OFPFW_NW_TOS},
};

UInt32 OriginalMatch::standardWildcards() const {
  UInt32 result = 0;
  UInt32 wc = wildcards;

  for (auto &entry : WildcardMap) {
    if (wc & entry.original) {
      result |= entry.standard;
    }
  }

  result |= (SW::OFPFW_MPLS_TC | SW::OFPFW_MPLS_LABEL);

  // Client must still check OW::OFPFW_DL_SRC and OW::OFPFW_DL_DST

  return result;
}

std::string OriginalMatch::toString() const {
  // A '*' after a value indicates the value is a wildcard.
  auto wildcard = [this](Wildcards wc) -> char {
    return wildcards & wc ? '*' : ' ';
  };

  std::stringstream ss;
  ss << "in_port: " << in_port << wildcard(OFPFW_IN_PORT) << '\n';
  ss << "dl_src: " << dl_src << wildcard(OFPFW_DL_SRC) << '\n';
  ss << "dl_dst: " << dl_dst << wildcard(OFPFW_DL_DST) << '\n';
  ss << "dl_vlan: " << dl_vlan << wildcard(OFPFW_DL_VLAN) << '\n';
  ss << "dl_vlan_pcp: " << static_cast<unsigned>(dl_vlan_pcp)
     << wildcard(OFPFW_DL_VLAN_PCP) << '\n';
  ss << "dl_type: " << dl_type << wildcard(OFPFW_DL_TYPE) << '\n';
  ss << "nw_tos: " << static_cast<unsigned>(nw_tos) << wildcard(OFPFW_NW_TOS)
     << '\n';
  ss << "nw_proto: " << static_cast<unsigned>(nw_proto)
     << wildcard(OFPFW_NW_PROTO) << '\n';
  ss << "nw_src: " << nw_src << '/' << nw_src_mask() << '\n';
  ss << "nw_dst: " << nw_dst << '/' << nw_dst_mask() << '\n';
  ss << "tp_src: " << tp_src << wildcard(OFPFW_TP_SRC) << '\n';
  ss << "tp_dst: " << tp_dst << wildcard(OFPFW_TP_DST) << '\n';
  return ss.str();
}
