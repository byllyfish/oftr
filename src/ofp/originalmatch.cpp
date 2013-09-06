#include "ofp/originalmatch.h"
#include "ofp/standardmatch.h"
#include <iterator>

namespace ofp {        // <namespace ofp>
namespace deprecated { // <namespace deprecated>

using OW = OriginalMatch::Wildcards;
using SW = StandardMatch::Wildcards;

OriginalMatch::OriginalMatch(const OXMRange &range)
{
    std::memset(this, 0, sizeof(OriginalMatch));
    
    UInt32 wc = OFPFW_ALL;
    for (auto &item : range) {
        switch (item.type()) {
        case OFB_IN_PORT::type() :
            in_port = UInt16_narrow_cast(item.value<OFB_IN_PORT>());
            wc &= ~OFPFW_IN_PORT;
            break;
        case OFB_ETH_SRC::type() :
            dl_src = item.value<OFB_ETH_SRC>();
            wc &= ~OFPFW_DL_SRC;
            break;
        case OFB_ETH_DST::type() :
            dl_dst = item.value<OFB_ETH_DST>();
            wc &= ~OFPFW_DL_DST;
            break;
        case OFB_ETH_TYPE::type() :
            dl_type = item.value<OFB_ETH_TYPE>();
            wc &= ~OFPFW_DL_TYPE;
            break;
        case OFB_VLAN_VID::type() :
            dl_vlan = item.value<OFB_VLAN_VID>();
            wc &= ~OFPFW_DL_VLAN;
            break;
        case OFB_VLAN_PCP::type() :
            dl_vlan_pcp = item.value<OFB_VLAN_PCP>();
            wc &= ~OFPFW_DL_VLAN_PCP;
            break;
        case OFB_IP_DSCP::type() :
            nw_tos = item.value<OFB_IP_DSCP>();
            wc &= ~OFPFW_NW_TOS;
            break;
        case OFB_IP_PROTO::type() :
            nw_proto = item.value<OFB_IP_PROTO>();
            wc &= ~OFPFW_NW_PROTO;
            break;
        case OFB_IPV4_SRC::type() :
            nw_src = item.value<OFB_IPV4_SRC>();
            set_nw_src_mask(32);
            break;
        case OFB_IPV4_SRC::type().withMask() :
            nw_src = item.value<OFB_IPV4_SRC>();
            set_nw_src_mask(item.mask<OFB_IPV4_SRC>().value().prefix());
            break;
        case OFB_IPV4_DST::type() :
            nw_dst = item.value<OFB_IPV4_DST>();
            set_nw_dst_mask(32);
            break;
        case OFB_IPV4_DST::type().withMask() :
            nw_dst = item.value<OFB_IPV4_DST>();
            set_nw_dst_mask(item.mask<OFB_IPV4_DST>().value().prefix());
            break;
        case OFB_TCP_SRC::type() :
            tp_src = item.value<OFB_TCP_SRC>();
            wc &= ~OFPFW_TP_SRC;
            break;
        case OFB_TCP_DST::type() :
            tp_dst = item.value<OFB_TCP_DST>();
            wc &= ~OFPFW_TP_DST;
            break;
        case OFB_UDP_SRC::type() :
            tp_src = item.value<OFB_UDP_SRC>();
            wc &= ~OFPFW_TP_SRC;
            break;
        case OFB_UDP_DST::type() :
            tp_dst = item.value<OFB_UDP_DST>();
            wc &= ~OFPFW_TP_DST;
            break;
        default:
            break;
        }
    }
    wildcards = wc;
}

struct WildcardInfo {
    OW original;
    SW standard;
};

static WildcardInfo WildcardMap[] = {
    { OW::OFPFW_IN_PORT, SW::OFPFW_IN_PORT },
    { OW::OFPFW_DL_VLAN, SW::OFPFW_DL_VLAN },
    { OW::OFPFW_DL_TYPE, SW::OFPFW_DL_TYPE },
    { OW::OFPFW_NW_PROTO, SW::OFPFW_NW_PROTO },
    { OW::OFPFW_TP_SRC, SW::OFPFW_TP_SRC },
    { OW::OFPFW_TP_DST, SW::OFPFW_TP_DST },
    { OW::OFPFW_DL_VLAN_PCP, SW::OFPFW_DL_VLAN_PCP },
    { OW::OFPFW_NW_TOS, SW::OFPFW_NW_TOS },
};

UInt32 OriginalMatch::standardWildcards() const
{
    UInt32 result = 0;
    UInt32 wc = wildcards;

    for (auto &entry : WildcardMap) {
        if (wc & entry.original) {
            result |= entry.standard;
        }
    }

    // TODO Handle OW::OFPFW_DL_SRC and OW::OFPFW_DL_DST

    return result;
}

} // </namespace deprecated>
} // </namespace ofp>
