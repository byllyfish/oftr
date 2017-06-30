// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/standardmatch.h"
#include "ofp/originalmatch.h"
#include "ofp/oxmlist.h"

using namespace ofp;
using namespace ofp::deprecated;

StandardMatch::StandardMatch(const OXMRange &range) {
  UInt32 wc = wildcards;
  for (auto &item : range) {
    switch (item.type()) {
      case OFB_IN_PORT::type():
        in_port = item.value<OFB_IN_PORT>().value();
        wc &= ~OFPFW_IN_PORT;
        break;
      case OFB_ETH_SRC::type():
        dl_src = item.value<OFB_ETH_SRC>();
        dl_src_mask.setAllOnes();
        break;
      case OFB_ETH_SRC::typeWithMask():
        dl_src = item.value<OFB_ETH_SRC>();
        dl_src_mask = item.mask<OFB_ETH_SRC>();
        break;
      case OFB_ETH_DST::type():
        dl_dst = item.value<OFB_ETH_DST>();
        dl_dst_mask.setAllOnes();
        break;
      case OFB_ETH_DST::typeWithMask():
        dl_dst = item.value<OFB_ETH_DST>();
        dl_dst_mask = item.mask<OFB_ETH_DST>();
        break;
      case OFB_VLAN_VID::type(): {
        UInt16 vlan = item.value<OFB_VLAN_VID>().value();
        dl_vlan =
            (vlan == OFPVID_NONE ? v1::OFPVID_NONE : (vlan & ~OFPVID_PRESENT));
        wc &= ~OFPFW_DL_VLAN;
        break;
      }
      case OFB_VLAN_VID::typeWithMask(): {
        UInt16 vlan = item.value<OFB_VLAN_VID>().value();
        UInt16 mask = item.mask<OFB_VLAN_VID>().value();
        if (vlan == OFPVID_PRESENT && mask == OFPVID_PRESENT) {
          dl_vlan = v1::OFPVID_PRESENT;
        } else {
          if (mask != 0xffff) {
            log_warning(
                "StandardMatch: VLAN mask not supported. Reverting to specific "
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
      case OFB_ETH_TYPE::type():
        dl_type = item.value<OFB_ETH_TYPE>();
        wc &= ~OFPFW_DL_TYPE;
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
        nw_src_mask.setAllOnes();
        break;
      case OFB_IPV4_SRC::typeWithMask():
        nw_src = item.value<OFB_IPV4_SRC>();
        nw_src_mask = item.mask<OFB_IPV4_SRC>();
        break;
      case OFB_IPV4_DST::type():
        nw_dst = item.value<OFB_IPV4_DST>();
        nw_dst_mask.setAllOnes();
        break;
      case OFB_IPV4_DST::typeWithMask():
        nw_dst = item.value<OFB_IPV4_DST>();
        nw_dst_mask = item.mask<OFB_IPV4_DST>();
        break;
      case OFB_TCP_SRC::type():
        tp_src = item.value<OFB_TCP_SRC>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_UDP_SRC::type():
        tp_src = item.value<OFB_UDP_SRC>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_ICMPV4_TYPE::type():
        tp_src = item.value<OFB_ICMPV4_TYPE>();
        wc &= ~OFPFW_TP_SRC;
        break;
      case OFB_TCP_DST::type():
        tp_dst = item.value<OFB_TCP_DST>();
        wc &= ~OFPFW_TP_DST;
        break;
      case OFB_UDP_DST::type():
        tp_dst = item.value<OFB_UDP_DST>();
        wc &= ~OFPFW_TP_DST;
        break;
      case OFB_ICMPV4_CODE::type():
        tp_dst = item.value<OFB_ICMPV4_CODE>();
        wc &= ~OFPFW_TP_DST;
        break;
      case OFB_MPLS_LABEL::type():
        mpls_label = item.value<OFB_MPLS_LABEL>();
        wc &= ~OFPFW_MPLS_LABEL;
        break;
      case OFB_MPLS_TC::type():
        mpls_tc = item.value<OFB_MPLS_TC>();
        wc &= ~OFPFW_MPLS_TC;
        break;
      case OFB_METADATA::type():
        metadata = item.value<OFB_METADATA>();
        metadata_mask = ~0ULL;
        break;
      case OFB_METADATA::typeWithMask():
        metadata = item.value<OFB_METADATA>();
        metadata_mask = item.mask<OFB_METADATA>();
        break;
      case OFB_ARP_OP::type():
        nw_proto = UInt8_narrow_cast(item.value<OFB_ARP_OP>());
        wc &= ~OFPFW_NW_PROTO;
        break;
      case OFB_ARP_SPA::type():
        nw_src = item.value<OFB_ARP_SPA>();
        nw_src_mask.setAllOnes();
        break;
      case OFB_ARP_SPA::typeWithMask():
        nw_src = item.value<OFB_ARP_SPA>();
        nw_src_mask = item.mask<OFB_ARP_SPA>();
        break;
      case OFB_ARP_TPA::type():
        nw_dst = item.value<OFB_ARP_TPA>();
        nw_dst_mask.setAllOnes();
        break;
      case OFB_ARP_TPA::typeWithMask():
        nw_dst = item.value<OFB_ARP_TPA>();
        nw_dst_mask = item.mask<OFB_ARP_TPA>();
        break;
      default:
        log_debug("StandardMatch: Unexpected oxm type:", item.type());
        break;
    }
  }
  wildcards = wc;
}

StandardMatch::StandardMatch(const OriginalMatch &match) {
  UInt32 origWildcards = match.wildcards;

  in_port = match.in_port;
  wildcards = match.standardWildcards();

  dl_src = match.dl_src;
  if (!(origWildcards & OriginalMatch::OFPFW_DL_SRC)) {
    dl_src_mask.setAllOnes();
  } else {
    dl_src_mask.clear();
  }

  dl_dst = match.dl_dst;
  if (!(origWildcards & OriginalMatch::OFPFW_DL_DST)) {
    dl_dst_mask.setAllOnes();
  } else {
    dl_dst_mask.clear();
  }

  dl_vlan = match.dl_vlan;
  dl_vlan_pcp = match.dl_vlan_pcp;
  dl_type = match.dl_type;
  nw_tos = match.nw_tos;
  nw_proto = match.nw_proto;
  nw_src = match.nw_src;
  nw_src_mask = match.nw_src_mask();
  nw_dst = match.nw_dst;
  nw_dst_mask = match.nw_dst_mask();
  tp_src = match.tp_src;
  tp_dst = match.tp_dst;
  mpls_label = 0;  // must be wildcarded
  mpls_tc = 0;     // must be wildcarded
  metadata = 0;
  metadata_mask = 0;

  // log_debug("Convert original match to standard match:");
  // log_debug("origmatch:\n", match.toString());
  // log_debug("stdmatch:\n", toString());
}

OXMList StandardMatch::toOXMList() const {
  OXMList list;
  UInt32 wc = wildcards;

  if (!(wc & OFPFW_IN_PORT)) {
    list.add(OFB_IN_PORT{in_port});
  }

  if (metadata_mask) {
    list.add(OFB_METADATA{metadata}, OFB_METADATA{metadata_mask});
  }

  if (dl_src_mask.valid()) {
    if (dl_src_mask.isBroadcast())
      list.add(OFB_ETH_SRC{dl_src});
    else
      list.add(OFB_ETH_SRC{dl_src}, OFB_ETH_SRC{dl_src_mask});
  }

  if (dl_dst_mask.valid()) {
    if (dl_dst_mask.isBroadcast())
      list.add(OFB_ETH_DST{dl_dst});
    else
      list.add(OFB_ETH_DST{dl_dst}, OFB_ETH_DST{dl_dst_mask});
  }

  if (!(wc & OFPFW_DL_VLAN)) {
    if (dl_vlan == v1::OFPVID_NONE) {  // Vlan not present
      list.add(OFB_VLAN_VID{OFPVID_NONE});
    } else if (dl_vlan == v1::OFPVID_PRESENT) {  // Vlan present, any value
      list.add(OFB_VLAN_VID{OFPVID_PRESENT}, OFB_VLAN_VID{OFPVID_PRESENT});
    } else {  // Vlan present, specific value
      list.add(OFB_VLAN_VID(dl_vlan | OFPVID_PRESENT));
    }
  }

  if (!(wc & OFPFW_DL_VLAN_PCP)) {
    list.add(OFB_VLAN_PCP{dl_vlan_pcp});
  }

  if (!(wc & OFPFW_DL_TYPE)) {
    list.add(OFB_ETH_TYPE{dl_type});

    // If dl_type is ARP (0x0806), process fields differently.
    if (dl_type == DATALINK_ARP) {
      convertDatalinkARP(wc, &list);
      return list;
    }
  }

  if (!(wc & OFPFW_NW_TOS)) {
    list.add(OFB_IP_DSCP{nw_tos});
  }

  if (!(wc & OFPFW_NW_PROTO)) {
    list.add(OFB_IP_PROTO{nw_proto});
  }

  if (nw_src_mask.valid()) {
    if (nw_src_mask.isBroadcast()) {
      list.add(OFB_IPV4_SRC{nw_src});
    } else {
      list.add(OFB_IPV4_SRC{nw_src}, OFB_IPV4_SRC{nw_src_mask});
    }
  }

  if (nw_dst_mask.valid()) {
    if (nw_dst_mask.isBroadcast()) {
      list.add(OFB_IPV4_DST{nw_dst});
    } else {
      list.add(OFB_IPV4_DST{nw_dst}, OFB_IPV4_DST{nw_dst_mask});
    }
  }

  if (!(wc & OFPFW_TP_SRC)) {
    if (wc & OFPFW_NW_PROTO) {
      log_warning(
          "StandardMatch::toOXMList: OFPFW_TP_SRC is missing OFPFW_NW_PROTO; "
          "default to TCP");
      list.add(OFB_TCP_SRC{tp_src});
    } else {
      switch (nw_proto) {
        case PROTOCOL_TCP:
          list.add(OFB_TCP_SRC{tp_src});
          break;
        case PROTOCOL_UDP:
          list.add(OFB_UDP_SRC{tp_src});
          break;
        case PROTOCOL_ICMP:
          list.add(OFB_ICMPV4_TYPE{UInt8_narrow_cast(tp_src)});
          break;
        case PROTOCOL_SCTP:
          list.add(OFB_SCTP_SRC{tp_src});
          break;
        default:
          log_warning(
              "StandardMatch::toOXMList: OFPFW_TP_SRC has unsupported "
              "OFPFW_NW_PROTO:",
              nw_proto);
          list.add(OFB_TCP_SRC{tp_src});
          break;
      }
    }
  }

  if (!(wc & OFPFW_TP_DST)) {
    if ((wc & OFPFW_NW_PROTO)) {
      log_warning(
          "StandardMatch::toOXMList: OFPFW_TP_DST is missing OFPFW_NW_PROTO; "
          "default to TCP");
      list.add(OFB_TCP_DST{tp_dst});
    } else {
      switch (nw_proto) {
        case PROTOCOL_TCP:
          list.add(OFB_TCP_DST{tp_dst});
          break;
        case PROTOCOL_UDP:
          list.add(OFB_UDP_DST{tp_dst});
          break;
        case PROTOCOL_ICMP:
          list.add(OFB_ICMPV4_CODE{UInt8_narrow_cast(tp_dst)});
          break;
        case PROTOCOL_SCTP:
          list.add(OFB_SCTP_DST{tp_dst});
          break;
        default:
          log_warning(
              "StandardMatch::toOXMList: OFPFW_TP_DST has unsupported "
              "OFPFW_NW_PROTO:",
              nw_proto);
          list.add(OFB_TCP_DST{tp_dst});
          break;
      }
    }
  }

  if (!(wc & OFPFW_MPLS_LABEL)) {
    list.add(OFB_MPLS_LABEL{mpls_label});
  }

  if (!(wc & OFPFW_MPLS_TC)) {
    list.add(OFB_MPLS_TC{mpls_tc});
  }

  return list;
}

void StandardMatch::convertDatalinkARP(UInt32 wc, OXMList *list) const {
  assert(dl_type == DATALINK_ARP);

  // nw_proto is ARP_OP (lower 8 bits)
  if (!(wc & OFPFW_NW_PROTO)) {
    list->add(OFB_ARP_OP{Big16{nw_proto}});
  }

  // nw_src is ARP_SPA
  if (nw_src_mask.valid()) {
    if (nw_src_mask.isBroadcast()) {
      list->add(OFB_ARP_SPA{nw_src});
    } else {
      list->add(OFB_ARP_SPA{nw_src}, OFB_ARP_SPA{nw_src_mask});
    }
  }

  // nw_dst is ARP_TPA
  if (nw_dst_mask.valid()) {
    if (nw_dst_mask.isBroadcast()) {
      list->add(OFB_ARP_TPA{nw_dst});
    } else {
      list->add(OFB_ARP_TPA{nw_dst}, OFB_ARP_TPA{nw_dst_mask});
    }
  }
}

std::string StandardMatch::toString() const {
  // A '*' after a value indicates the value is a wildcard.
  auto wildcard = [this](Wildcards wc) -> char {
    return wildcards & wc ? '*' : ' ';
  };

  std::string buf;
  llvm::raw_string_ostream ss{buf};
  ss << "in_port: " << in_port << wildcard(OFPFW_IN_PORT) << '\n';
  ss << "dl_src: " << dl_src << '/' << dl_src_mask << '\n';
  ss << "dl_dst: " << dl_dst << '/' << dl_dst_mask << '\n';
  ss << "dl_vlan: " << dl_vlan << wildcard(OFPFW_DL_VLAN) << '\n';
  ss << "dl_vlan_pcp: " << static_cast<unsigned>(dl_vlan_pcp)
     << wildcard(OFPFW_DL_VLAN_PCP) << '\n';
  ss << "dl_type: " << dl_type << wildcard(OFPFW_DL_TYPE) << '\n';
  ss << "nw_tos: " << static_cast<unsigned>(nw_tos) << wildcard(OFPFW_NW_TOS)
     << '\n';
  ss << "nw_proto: " << static_cast<unsigned>(nw_proto)
     << wildcard(OFPFW_NW_PROTO) << '\n';
  ss << "nw_src: " << nw_src << '/' << nw_src_mask << '\n';
  ss << "nw_dst: " << nw_dst << '/' << nw_dst_mask << '\n';
  ss << "tp_src: " << tp_src << wildcard(OFPFW_TP_SRC) << '\n';
  ss << "tp_dst: " << tp_dst << wildcard(OFPFW_TP_DST) << '\n';
  ss << "mpls_label: " << mpls_label << wildcard(OFPFW_MPLS_LABEL) << '\n';
  ss << "mpls_tc: " << static_cast<unsigned>(mpls_tc) << wildcard(OFPFW_MPLS_TC)
     << '\n';
  ss << "metadata: " << metadata << '/' << metadata_mask << '\n';
  return ss.str();
}
