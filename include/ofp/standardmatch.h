//  ===== ---- ofp/standardmatch.h -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the StandardMatch class.
//  ===== ------------------------------------------------------------ =====  //

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

struct OriginalMatch;

enum {
    OFPMT_STANDARD_LENGTH = 88
};

struct StandardMatch {

    enum Wildcards : UInt32 {
        OFPFW_IN_PORT = 1 << 0,     //< Switch input port
        OFPFW_DL_VLAN = 1 << 1,     //< VLAN id
        OFPFW_DL_VLAN_PCP = 1 << 2, //< VLAN priority
        OFPFW_DL_TYPE = 1 << 3,     //< Ethernet frame type
        OFPFW_NW_TOS = 1 << 4,      //< IP ToS (DSCP field, 6 bits)
        OFPFW_NW_PROTO = 1 << 5,    //< IP protocol
        OFPFW_TP_SRC = 1 << 6,      //< TCP/UDP/SCTP source port
        OFPFW_TP_DST = 1 << 7,      //< TCP/UDP/SCTP destination port
        OFPFW_MPLS_LABEL = 1 << 8,  //< MPLS label
        OFPFW_MPLS_TC = 1 << 9,     //< MPLS TC
        OFPFW_ALL = ((1 << 10) - 1) //< Wildcard all fields
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
    StandardMatch(const OXMRange &range);
    StandardMatch(const OriginalMatch &match);

    OXMList toOXMList() const;
};

static_assert(sizeof(StandardMatch) == OFPMT_STANDARD_LENGTH, "Unexpected size.");

} // </namespace deprecated>
} // </namespace ofp>

#endif // OFP_STANDARDMATCH_H

