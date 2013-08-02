

#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include <iostream>

using namespace ofp;

static void WritePrereq(const char *name, const UInt8 *data, size_t len)
{
  std::cout << "static const ofp::UInt8 data_" << name << "[" << len << "]={";
  for (size_t i = 0; i < len; ++i) {
    std::cout << unsigned(data[i]) << ",";
  }
  std::cout << "};\n";
  std::cout << "const ofp::OXMRange ofp::OXMPrereq_" << name << "{data_" << name << "," << len <<"};\n";
}

int main() {
  std::cout << "#include \"ofp/oxmfields.h\"\n";
  
{
OXMList list;
list.addPrereq(OFB_IN_PORT{0},OFB_IN_PORT{0});
list.addPrereq(OFB_IN_PORT{0},OFB_IN_PORT{0});
WritePrereq("OFB_IN_PHY_PORT", list.data(), list.size());
}
{
OXMList list;
list.addPrereq(OFB_VLAN_VID{0x1000},OFB_VLAN_VID{0x1000});
list.addPrereq(OFB_VLAN_VID{0x1000},OFB_VLAN_VID{0x1000});
WritePrereq("OFB_VLAN_PCP", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IP_DSCP", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IP_ECN", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IP_PROTO", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
WritePrereq("OFB_IPV4_SRC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
WritePrereq("OFB_IPV4_DST", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{6});
WritePrereq("OFB_TCP_SRC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{6});
WritePrereq("OFB_TCP_DST", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{17});
WritePrereq("OFB_UDP_SRC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{17});
WritePrereq("OFB_UDP_DST", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{132});
WritePrereq("OFB_SCTP_SRC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{132});
WritePrereq("OFB_SCTP_DST", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{1});
WritePrereq("OFB_ICMPV4_TYPE", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{1});
WritePrereq("OFB_ICMPV4_CODE", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0806});
WritePrereq("OFB_ARP_OP", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0806});
WritePrereq("OFB_ARP_SPA", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0806});
WritePrereq("OFB_ARP_TPA", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0806});
WritePrereq("OFB_ARP_SHA", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0806});
WritePrereq("OFB_ARP_THA", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IPV6_SRC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IPV6_DST", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IPV6_FLABEL", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{58});
WritePrereq("OFB_ICMPV6_TYPE", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{58});
WritePrereq("OFB_ICMPV6_CODE", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{58});
list.add(OFB_ICMPV6_TYPE{135});
list.add(OFB_ICMPV6_TYPE{136});
WritePrereq("OFB_IPV6_ND_TARGET", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{58});
list.add(OFB_ICMPV6_TYPE{135});
WritePrereq("OFB_IPV6_ND_SLL", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x0800});
list.add(OFB_ETH_TYPE{0x86dd});
list.add(OFB_IP_PROTO{58});
list.add(OFB_ICMPV6_TYPE{136});
WritePrereq("OFB_IPV6_ND_TLL", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x8847});
list.add(OFB_ETH_TYPE{0x8848});
WritePrereq("OFB_MPLS_LABEL", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x8847});
list.add(OFB_ETH_TYPE{0x8848});
WritePrereq("OFB_MPLS_TC", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x8847});
list.add(OFB_ETH_TYPE{0x8848});
WritePrereq("OFP_MPLS_BOS", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x88E7});
WritePrereq("OFB_PBB_ISID", list.data(), list.size());
}
{
OXMList list;
list.add(OFB_ETH_TYPE{0x86dd});
WritePrereq("OFB_IPV6_EXTHDR", list.data(), list.size());
}
}
