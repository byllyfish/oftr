#include "ofp/oxmfields.h"
static const ofp::UInt8 data_OFB_IN_PHY_PORT[12]={128,0,1,8,0,0,0,0,0,0,0,0,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IN_PHY_PORT{data_OFB_IN_PHY_PORT,12};
static const ofp::UInt8 data_OFB_VLAN_PCP[8]={128,0,13,4,16,0,16,0,};
const ofp::OXMRange ofp::OXMPrereq_OFB_VLAN_PCP{data_OFB_VLAN_PCP,8};
static const ofp::UInt8 data_OFB_IP_DSCP[12]={128,0,10,2,8,0,128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IP_DSCP{data_OFB_IP_DSCP,12};
static const ofp::UInt8 data_OFB_IP_ECN[12]={128,0,10,2,8,0,128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IP_ECN{data_OFB_IP_ECN,12};
static const ofp::UInt8 data_OFB_IP_PROTO[12]={128,0,10,2,8,0,128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IP_PROTO{data_OFB_IP_PROTO,12};
static const ofp::UInt8 data_OFB_IPV4_SRC[6]={128,0,10,2,8,0,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV4_SRC{data_OFB_IPV4_SRC,6};
static const ofp::UInt8 data_OFB_IPV4_DST[6]={128,0,10,2,8,0,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV4_DST{data_OFB_IPV4_DST,6};
static const ofp::UInt8 data_OFB_TCP_SRC[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_TCP_SRC{data_OFB_TCP_SRC,17};
static const ofp::UInt8 data_OFB_TCP_DST[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_TCP_DST{data_OFB_TCP_DST,17};
static const ofp::UInt8 data_OFB_UDP_SRC[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,17,};
const ofp::OXMRange ofp::OXMPrereq_OFB_UDP_SRC{data_OFB_UDP_SRC,17};
static const ofp::UInt8 data_OFB_UDP_DST[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,17,};
const ofp::OXMRange ofp::OXMPrereq_OFB_UDP_DST{data_OFB_UDP_DST,17};
static const ofp::UInt8 data_OFB_SCTP_SRC[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,132,};
const ofp::OXMRange ofp::OXMPrereq_OFB_SCTP_SRC{data_OFB_SCTP_SRC,17};
static const ofp::UInt8 data_OFB_SCTP_DST[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,132,};
const ofp::OXMRange ofp::OXMPrereq_OFB_SCTP_DST{data_OFB_SCTP_DST,17};
static const ofp::UInt8 data_OFB_ICMPV4_TYPE[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,1,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ICMPV4_TYPE{data_OFB_ICMPV4_TYPE,17};
static const ofp::UInt8 data_OFB_ICMPV4_CODE[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,1,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ICMPV4_CODE{data_OFB_ICMPV4_CODE,17};
static const ofp::UInt8 data_OFB_ARP_OP[6]={128,0,10,2,8,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ARP_OP{data_OFB_ARP_OP,6};
static const ofp::UInt8 data_OFB_ARP_SPA[6]={128,0,10,2,8,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ARP_SPA{data_OFB_ARP_SPA,6};
static const ofp::UInt8 data_OFB_ARP_TPA[6]={128,0,10,2,8,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ARP_TPA{data_OFB_ARP_TPA,6};
static const ofp::UInt8 data_OFB_ARP_SHA[6]={128,0,10,2,8,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ARP_SHA{data_OFB_ARP_SHA,6};
static const ofp::UInt8 data_OFB_ARP_THA[6]={128,0,10,2,8,6,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ARP_THA{data_OFB_ARP_THA,6};
static const ofp::UInt8 data_OFB_IPV6_SRC[6]={128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_SRC{data_OFB_IPV6_SRC,6};
static const ofp::UInt8 data_OFB_IPV6_DST[6]={128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_DST{data_OFB_IPV6_DST,6};
static const ofp::UInt8 data_OFB_IPV6_FLABEL[6]={128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_FLABEL{data_OFB_IPV6_FLABEL,6};
static const ofp::UInt8 data_OFB_ICMPV6_TYPE[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,58,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ICMPV6_TYPE{data_OFB_ICMPV6_TYPE,17};
static const ofp::UInt8 data_OFB_ICMPV6_CODE[17]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,58,};
const ofp::OXMRange ofp::OXMPrereq_OFB_ICMPV6_CODE{data_OFB_ICMPV6_CODE,17};
static const ofp::UInt8 data_OFB_IPV6_ND_TARGET[27]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,58,128,0,58,1,135,128,0,58,1,136,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_ND_TARGET{data_OFB_IPV6_ND_TARGET,27};
static const ofp::UInt8 data_OFB_IPV6_ND_SLL[22]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,58,128,0,58,1,135,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_ND_SLL{data_OFB_IPV6_ND_SLL,22};
static const ofp::UInt8 data_OFB_IPV6_ND_TLL[22]={128,0,10,2,8,0,128,0,10,2,134,221,128,0,20,1,58,128,0,58,1,136,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_ND_TLL{data_OFB_IPV6_ND_TLL,22};
static const ofp::UInt8 data_OFB_MPLS_LABEL[12]={128,0,10,2,136,71,128,0,10,2,136,72,};
const ofp::OXMRange ofp::OXMPrereq_OFB_MPLS_LABEL{data_OFB_MPLS_LABEL,12};
static const ofp::UInt8 data_OFB_MPLS_TC[12]={128,0,10,2,136,71,128,0,10,2,136,72,};
const ofp::OXMRange ofp::OXMPrereq_OFB_MPLS_TC{data_OFB_MPLS_TC,12};
static const ofp::UInt8 data_OFP_MPLS_BOS[12]={128,0,10,2,136,71,128,0,10,2,136,72,};
const ofp::OXMRange ofp::OXMPrereq_OFP_MPLS_BOS{data_OFP_MPLS_BOS,12};
static const ofp::UInt8 data_OFB_PBB_ISID[6]={128,0,10,2,136,231,};
const ofp::OXMRange ofp::OXMPrereq_OFB_PBB_ISID{data_OFB_PBB_ISID,6};
static const ofp::UInt8 data_OFB_IPV6_EXTHDR[6]={128,0,10,2,134,221,};
const ofp::OXMRange ofp::OXMPrereq_OFB_IPV6_EXTHDR{data_OFB_IPV6_EXTHDR,6};
const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[40] = {
{ "OFB_IN_PORT", nullptr, 67108992, 0},
{ "OFB_IN_PHY_PORT", &ofp::OXMPrereq_OFB_IN_PHY_PORT, 67240064, 0},
{ "OFB_METADATA", nullptr, 134480000, 1},
{ "OFB_ETH_DST", nullptr, 101056640, 1},
{ "OFB_ETH_SRC", nullptr, 101187712, 1},
{ "OFB_ETH_TYPE", nullptr, 34209920, 0},
{ "OFB_VLAN_VID", nullptr, 34340992, 1},
{ "OFB_VLAN_PCP", &ofp::OXMPrereq_OFB_VLAN_PCP, 17694848, 0},
{ "OFB_IP_DSCP", &ofp::OXMPrereq_OFB_IP_DSCP, 17825920, 0},
{ "OFB_IP_ECN", &ofp::OXMPrereq_OFB_IP_ECN, 17956992, 0},
{ "OFB_IP_PROTO", &ofp::OXMPrereq_OFB_IP_PROTO, 18088064, 0},
{ "OFB_IPV4_SRC", &ofp::OXMPrereq_OFB_IPV4_SRC, 68550784, 1},
{ "OFB_IPV4_DST", &ofp::OXMPrereq_OFB_IPV4_DST, 68681856, 1},
{ "OFB_TCP_SRC", &ofp::OXMPrereq_OFB_TCP_SRC, 35258496, 0},
{ "OFB_TCP_DST", &ofp::OXMPrereq_OFB_TCP_DST, 35389568, 0},
{ "OFB_UDP_SRC", &ofp::OXMPrereq_OFB_UDP_SRC, 35520640, 0},
{ "OFB_UDP_DST", &ofp::OXMPrereq_OFB_UDP_DST, 35651712, 0},
{ "OFB_SCTP_SRC", &ofp::OXMPrereq_OFB_SCTP_SRC, 35782784, 0},
{ "OFB_SCTP_DST", &ofp::OXMPrereq_OFB_SCTP_DST, 35913856, 0},
{ "OFB_ICMPV4_TYPE", &ofp::OXMPrereq_OFB_ICMPV4_TYPE, 19267712, 0},
{ "OFB_ICMPV4_CODE", &ofp::OXMPrereq_OFB_ICMPV4_CODE, 19398784, 0},
{ "OFB_ARP_OP", &ofp::OXMPrereq_OFB_ARP_OP, 36307072, 0},
{ "OFB_ARP_SPA", &ofp::OXMPrereq_OFB_ARP_SPA, 69992576, 1},
{ "OFB_ARP_TPA", &ofp::OXMPrereq_OFB_ARP_TPA, 70123648, 1},
{ "OFB_ARP_SHA", &ofp::OXMPrereq_OFB_ARP_SHA, 103809152, 1},
{ "OFB_ARP_THA", &ofp::OXMPrereq_OFB_ARP_THA, 103940224, 1},
{ "OFB_IPV6_SRC", &ofp::OXMPrereq_OFB_IPV6_SRC, 271843456, 1},
{ "OFB_IPV6_DST", &ofp::OXMPrereq_OFB_IPV6_DST, 271974528, 1},
{ "OFB_IPV6_FLABEL", &ofp::OXMPrereq_OFB_IPV6_FLABEL, 54001792, 1},
{ "OFB_ICMPV6_TYPE", &ofp::OXMPrereq_OFB_ICMPV6_TYPE, 20578432, 0},
{ "OFB_ICMPV6_CODE", &ofp::OXMPrereq_OFB_ICMPV6_CODE, 20709504, 0},
{ "OFB_IPV6_ND_TARGET", &ofp::OXMPrereq_OFB_IPV6_ND_TARGET, 272498816, 0},
{ "OFB_IPV6_ND_SLL", &ofp::OXMPrereq_OFB_IPV6_ND_SLL, 104857728, 0},
{ "OFB_IPV6_ND_TLL", &ofp::OXMPrereq_OFB_IPV6_ND_TLL, 104988800, 0},
{ "OFB_MPLS_LABEL", &ofp::OXMPrereq_OFB_MPLS_LABEL, 54788224, 0},
{ "OFB_MPLS_TC", &ofp::OXMPrereq_OFB_MPLS_TC, 21364864, 0},
{ "OFP_MPLS_BOS", &ofp::OXMPrereq_OFP_MPLS_BOS, 21495936, 0},
{ "OFB_PBB_ISID", &ofp::OXMPrereq_OFB_PBB_ISID, 55181440, 1},
{ "OFB_TUNNEL_ID", nullptr, 139198592, 1},
{ "OFB_IPV6_EXTHDR", &ofp::OXMPrereq_OFB_IPV6_EXTHDR, 38666368, 1},
};
const size_t ofp::OXMTypeInfoArraySize=40;
