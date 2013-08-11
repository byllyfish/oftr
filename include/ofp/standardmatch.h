#ifndef OFP_STANDARDMATCH_H
#define OFP_STANDARDMATCH_H

#include "ofp/oxmfields.h"
#include "ofp/enetaddress.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/oxmlist.h"
#include "ofp/padding.h"
#include "ofp/log.h"
#include "ofp/constants.h"

namespace ofp {        // <namespace ofp>
namespace deprecated { // <namespace deprecated>

enum {
    OFPMT_STANDARD_LENGTH = 88
};

enum : UInt32{
    /**
     *  Switch input port
       */
    OFPFW_IN_PORT = 1 << 0,

    /**
     *  VLAN id
       */
    OFPFW_DL_VLAN = 1 << 1,

    /**
     *  VLAN priority
       */
    OFPFW_DL_VLAN_PCP = 1 << 2,

    /**
     *  Ethernet frame type
       */
    OFPFW_DL_TYPE = 1 << 3,

    /**
     *  IP ToS (DSCP field, 6 bits)
       */
    OFPFW_NW_TOS = 1 << 4,

    /**
     *  IP protocol
       */
    OFPFW_NW_PROTO = 1 << 5,

    /**
     *  TCP/UDP/SCTP source port
       */
    OFPFW_TP_SRC = 1 << 6,

    /**
     *  TCP/UDP/SCTP destination port
       */
    OFPFW_TP_DST = 1 << 7,

    /**
     *  MPLS label
       */
    OFPFW_MPLS_LABEL = 1 << 8,

    /**
     *  MPLS TC
       */
    OFPFW_MPLS_TC = 1 << 9,

    /**
     *  Wildcard all fields
        */
    OFPFW_ALL = ((1 << 10) - 1)
};

enum {
	TCP = 6,
	UDP = 17,
	SCTP = 132
};

struct StandardMatch {
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

    StandardMatch() = default;

    StandardMatch(const OXMRange &range) 
    {
        in_port = 0;
        std::memset(&dl_src, 0, sizeof(StandardMatch) - 12);

        UInt32 wc = wildcards;
        for (auto item : range) {
            switch (item.type()) {
                case OFB_IN_PORT::type():
                    in_port = item.value<OFB_IN_PORT>();
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
                default:
                    // FIXME
                    break;
            }
        }
        wildcards = wc;
    }

    OXMList toOXMList() const
    {
        OXMList list;
        UInt32 wc = wildcards;

        if (!(wc & OFPFW_IN_PORT)) {
            list.add(OFB_IN_PORT{in_port});
        }

        if (dl_src_mask.valid()) 
            list.add(OFB_ETH_SRC{dl_src}, OFB_ETH_SRC{dl_src_mask});

        if (dl_dst_mask.valid())
            list.add(OFB_ETH_DST{dl_dst}, OFB_ETH_DST{dl_dst_mask});

        if (!(wc & OFPFW_DL_VLAN)) {
            list.add(OFB_VLAN_VID{dl_vlan});
        }

        if (!(wc & OFPFW_DL_VLAN_PCP)) {
            list.add(OFB_VLAN_PCP{dl_vlan_pcp});
        }

        if (!(wc & OFPFW_DL_TYPE)) {
            list.add(OFB_ETH_TYPE{dl_type});
        }

        if (!(wc & OFPFW_NW_TOS)) {
            list.add(OFB_IP_DSCP{nw_tos});
        }

        if (!(wc & OFPFW_NW_PROTO)) {
            list.add(OFB_IP_PROTO{nw_proto});
        }

        if (nw_src_mask.valid())
            list.add(OFB_IPV4_SRC{nw_src}, OFB_IPV4_SRC{nw_src_mask});

        if (nw_dst_mask.valid())
            list.add(OFB_IPV4_DST{nw_dst}, OFB_IPV4_DST{nw_dst_mask});

        if (!(wc & OFPFW_TP_SRC)) {
            if (wc & nw_proto) {
                log::info("OFPFW_TP_SRC is missing OFPFW_NW_PROTO.");
            } else {
                switch (nw_proto) {
                case TCP:
                    list.add(OFB_TCP_SRC{tp_src});
                    break;
                case UDP:
                    list.add(OFB_UDP_SRC{tp_src});
                    break;
                case SCTP:
                    list.add(OFB_SCTP_SRC{tp_src});
                    break;
                default:
                    log::info("OFPFW_TP_SRC has unsupported OFPFW_NW_PROTO:",
                              nw_proto);
                    break;
                }
            }
        }

        if (!(wc & OFPFW_TP_DST)) {
            if ((wc & nw_proto)) {
                log::info("OFPFW_TP_DST is missing OFPFW_NW_PROTO.");
            } else {
                switch (nw_proto) {
                case TCP:
                    list.add(OFB_TCP_DST{tp_dst});
                    break;
                case UDP:
                    list.add(OFB_UDP_DST{tp_dst});
                    break;
                case SCTP:
                    list.add(OFB_SCTP_DST{tp_dst});
                    break;
                default:
                    log::info("OFPFW_TP_DST has unsupported OFPFW_NW_PROTO:",
                              nw_proto);
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

        if (metadata_mask)
            list.add(OFB_METADATA{metadata}, OFB_METADATA{metadata_mask});

        return list;
    }
};

static_assert(sizeof(StandardMatch) == OFPMT_STANDARD_LENGTH, "Unexpected size.");

} // </namespace deprecated>
} // </namespace ofp>

#endif // OFP_STANDARDMATCH_H

