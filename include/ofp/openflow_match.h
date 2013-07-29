#ifndef OFP_OPENFLOW_MATCH_H
#define OFP_OPENFLOW_MATCH_H

#include "ofp/byteorder.h"


namespace ofp { // <namespace ofp>
namespace spec { // <namespace spec>


const size_t OFP_ETH_ALEN = 6;

// The match type indicates the match structure (set of fields that compose the
// match) in use. The match type is placed in the type field at the beginning
// of all match structures. The "OpenFlow Extensible Match" type corresponds
// to OXM TLV format described below and must be supported by all OpenFlow
// switches. Extensions that define other match types may be published on the
// ONF wiki. Support for extensions is optional.

enum ofp_match_type : UInt16 {
	OFPMT_STANDARD = 0,				// Deprecated.
	OFPMT_OXM      = 1,				// OpenFlow Extensible Match.
};

struct ofp_match_header {
    Big<ofp_match_type> type;		// One of OFPMT_* 
    Big16 length;           		// Length of ofp_match (excluding padding)
    
    // Followed by:
	//	- Exactly (length - 4) (possibly 0) bytes containing OXM TLVs, then
	//	- Exactly ((length + 7)/8*8 - length) (between 0 and 7) bytes of
	//	  all-zero bytes
	// In summary, ofp_match is padded as needed, to make its overall size
	// a multiple of 8, to preserve alignment in structures using it.
	
//	Big8 oxm_fields[0];  			// 0 or more OXM match fields
    Big8 pad[4];            		// Zero bytes - see above for sizing
};
static_assert(sizeof(ofp_match_header) == 8, "Incorrect ofp_match_header size.");


// 7.2.3.3 OXM classes

/* OXM Class IDs.
 * The high order bit differentiate reserved classes from member classes.
 * Classes 0x0000 to 0x7FFF are member classes, allocated by ONF.
 * Classes 0x8000 to 0xFFFE are reserved classes, reserved for standardisation.
 */
enum ofp_oxm_class : UInt16 {
    OFPXMC_NXM_0          = 0x0000,/* Backward compatibility with NXM */
    OFPXMC_NXM_1          = 0x0001,/* Backward compatibility with NXM */
    OFPXMC_OPENFLOW_BASIC = 0x8000,/* Basic class for OpenFlow */
    OFPXMC_EXPERIMENTER   = 0xFFFF,/* Experimenter class */
};


struct ofp_oxm {
	Big_unaligned<ofp_oxm_class> oxm_type;	// 
	Big8 		    	oxm_field;   // includes mask bit
	Big8 		    	oxm_length;
//  Big8 				oxm_data[0];
};
static_assert(sizeof(ofp_oxm) == 4, "Incorrect ofp_oxm size.");





enum class ofp_flow_wildcards_02 : UInt32 {
	OFPFW_IN_PORT = 1 << 0, /* Switch input port. */
	OFPFW_DL_VLAN = 1 << 1, /* VLAN id. */
	OFPFW_DL_SRC = 1 << 2, /* Ethernet source address. */
	OFPFW_DL_DST = 1 << 3, /* Ethernet destination address. */
	OFPFW_DL_TYPE = 1 << 4, /* Ethernet frame type. */
	OFPFW_NW_PROTO = 1 << 5, /* IP protocol. */
	OFPFW_TP_SRC = 1 << 6, /* TCP/UDP source port. */
	OFPFW_TP_DST = 1 << 7, /* TCP/UDP destination port. */
	
	// IP source address wildcard bit count. 0 is exact match, 1 ignores the
	// LSB, 2 ignores the 2 least-significant bits, ..., 32 and higher wildcard
	// the entire field. This is the *opposite* of the usual convention where
	// e.g. /24 indicates that 8 bits (not 24 bits) are wildcarded.
	OFPFW_NW_SRC_SHIFT = 8,
	OFPFW_NW_SRC_BITS = 6,
	OFPFW_NW_SRC_MASK = ((1 << OFPFW_NW_SRC_BITS) - 1) << OFPFW_NW_SRC_SHIFT,
	OFPFW_NW_SRC_ALL = 32 << OFPFW_NW_SRC_SHIFT,
	
	// IP destination address wildcard bit count. Same format as source.
	OFPFW_NW_DST_SHIFT = 14,
	OFPFW_NW_DST_BITS = 6,
	OFPFW_NW_DST_MASK = ((1 << OFPFW_NW_DST_BITS) - 1) << OFPFW_NW_DST_SHIFT,
	OFPFW_NW_DST_ALL = 32 << OFPFW_NW_DST_SHIFT,
	
	OFPFW_DL_VLAN_PCP = 1 << 20, 		// VLAN priority.
	OFPFW_NW_TOS = 1 << 21, 			// IP ToS (DSCP field, 6 bits).
	
	// Wildcard all fields.
	OFPFW_ALL = ((1 << 22) - 1)
};


//  Fields to match against flows 
struct ofp_match_02 {
	Big<ofp_flow_wildcards_02> wildcards; 	// Wildcard fields. 
	Big16 in_port; 				// Input switch port. 
	Big8 dl_src[OFP_ETH_ALEN]; 	// Ethernet source address. 
	Big8 dl_dst[OFP_ETH_ALEN]; 	// Ethernet destination address. 
	Big16 dl_vlan; 				// Input VLAN id. 
	Big8 dl_vlan_pcp; 			// Input VLAN priority. 
	Big8 pad1[1]; 				// Align to 64-bits 
	Big16 dl_type; 				// Ethernet frame type. 
	Big8 nw_tos; 				// IP ToS (actually DSCP field, 6 bits). 
	Big8 nw_proto; 				// IP protocol or lower 8 bits of ARP opcode.
	Big8 pad2[2]; 				// Align to 64-bits 
	Big32 nw_src; 				// IP source address. 
	Big32 nw_dst; 				// IP destination address.
	Big16 tp_src; 				// TCP/UDP source port. 
	Big16 tp_dst; 				// TCP/UDP destination port. 
};
static_assert(sizeof(ofp_match_02) == 40, "Incorrect ofp_match_02 size.");


} // </namespace spec>
} // </namespace ofp>

#endif // OFP_OPENFLOW_MATCH_H
