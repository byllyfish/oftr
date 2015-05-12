// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_CONSTANTS_H_
#define OFP_CONSTANTS_H_

#include "ofp/types.h"
#include "ofp/constants_deprecated.h"
#include "ofp/constants_error.h"
#include "ofp/protocoliteratortype.h"

namespace ofp {

// Internet Protocol Transport Types
enum : UInt8 {
  PROTOCOL_ICMP = 1,
  PROTOCOL_TCP = 6,
  PROTOCOL_UDP = 17,
  PROTOCOL_ICMPV6 = 58,
  PROTOCOL_SCTP = 132,
};

enum : UInt16 {
  DATALINK_ARP = 0x0806,
  DATALINK_IPV4 = 0x0800,
  DATALINK_IPV6 = 0x86dd,
  DATALINK_LLDP = 0x88cc,
};

enum : size_t {
  // Used by ProtocolIterator template class.
  PROTOCOL_ITERATOR_SIZE_FIXED = 0xffff,
  PROTOCOL_ITERATOR_SIZE_CONDITIONAL = 0xfffe
};

enum {
  OFP_VERSION_1 = 0x01,
  OFP_VERSION_2 = 0x02,
  OFP_VERSION_3 = 0x03,
  OFP_VERSION_4 = 0x04,
  OFP_VERSION_5 = 0x05,
  OFP_VERSION_LAST = OFP_VERSION_4,
  OFP_VERSION_MAX_ALLOWED = 0x09
};

enum {
  OFP_DEFAULT_PORT = 6633,

  // Maximum size of an OpenFlow message.
  OFP_MAX_SIZE = 65535
};

enum : UInt16 { OFPVID_PRESENT = 0x1000, OFPVID_NONE = 0x0000 };

// NXM_NX_IP_FRAG
enum : UInt8 {
  NXM_FRAG_TYPE_NONE = 0x00,
  NXM_FRAG_TYPE_ANY = 0x01,
  NXM_FRAG_TYPE_LATER = 0x02,
};

enum { OFPMT_STANDARD = 0, OFPMT_OXM = 1 };

enum OFPType : UInt8 {
  // Symmetric messages. Use OFPT_EXPERIMENTER in place of OFPT_VENDOR for
  // earlier versions of the protocol.
  OFPT_HELLO = 0,
  OFPT_ERROR = 1,
  OFPT_ECHO_REQUEST = 2,
  OFPT_ECHO_REPLY = 3,
  OFPT_EXPERIMENTER = 4,

  // Switch configuration messages.
  OFPT_FEATURES_REQUEST = 5,
  OFPT_FEATURES_REPLY = 6,
  OFPT_GET_CONFIG_REQUEST = 7,
  OFPT_GET_CONFIG_REPLY = 8,
  OFPT_SET_CONFIG = 9,

  // Asynchronous messages from Switch.
  OFPT_PACKET_IN = 10,
  OFPT_FLOW_REMOVED = 11,
  OFPT_PORT_STATUS = 12,

  // Controller command messages.
  OFPT_PACKET_OUT = 13,
  OFPT_FLOW_MOD = 14,
  OFPT_GROUP_MOD = 15,  // Min: 1.1
  OFPT_PORT_MOD = 16,
  OFPT_TABLE_MOD = 17,  // Min: 1.1

  // Multipart messages. Use OFPT_MULTIPART_REQUEST in place of
  // OFPT_STATS_REQUEST and OFPT_MULTIPART_REPLY in place of OFPT_STATS_REPLY
  // for earlier versions of the protocol.
  OFPT_MULTIPART_REQUEST = 18,
  OFPT_MULTIPART_REPLY = 19,

  // Barrier messages.
  OFPT_BARRIER_REQUEST = 20,
  OFPT_BARRIER_REPLY = 21,

  // Queue Configuration messages.
  OFPT_QUEUE_GET_CONFIG_REQUEST = 22,
  OFPT_QUEUE_GET_CONFIG_REPLY = 23,

  // Controller role change request messages.
  OFPT_ROLE_REQUEST = 24,  // Min: 1.2
  OFPT_ROLE_REPLY = 25,    // Min: 1.2

  // Asynchronous message configuration messages.
  OFPT_GET_ASYNC_REQUEST = 26,  // Min: 1.3
  OFPT_GET_ASYNC_REPLY = 27,    // Min: 1.3
  OFPT_SET_ASYNC = 28,          // Min: 1.3

  // Meters and rate limiters configuration messages.
  OFPT_METER_MOD = 29,           // Min: 1.3
  OFPT_ROLE_STATUS = 30,         // Min: 1.4
  OFPT_TABLE_STATUS = 31,        // Min: 1.4
  OFPT_REQUESTFORWARD = 32,      // Min: 1.4
  OFPT_BUNDLE_CONTROL = 33,      // Min: 1.4
  OFPT_BUNDLE_ADD_MESSAGE = 34,  // Min: 1.4

  /// Used internally.
  OFPT_LAST = OFPT_BUNDLE_ADD_MESSAGE,
  OFPT_MAX_ALLOWED = 64,
  OFPT_UNSUPPORTED = 0xF9,
};

std::ostream &operator<<(std::ostream &os, OFPType type);

enum OFPBufferID : UInt32 { OFP_NO_BUFFER = 0xFFFFFFFFUL };

// Flags to indicate behavior of the physical port.  These flags are
// used in ofp_port to describe the current configuration.  They are
// used in the ofp_port_mod message to configure the port's behavior.

enum OFPPortConfigFlags : UInt32 {
  OFPPC_NONE = 0,
  OFPPC_PORT_DOWN = 1 << 0,
  OFPPC_NO_STP = 1 << 1,  // Version 1.0 only
  OFPPC_NO_RECV = 1 << 2,
  OFPPC_NO_RECV_STP = 1 << 3,
  OFPPC_NO_FLOOD = 1 << 4,  // Version 1.0 only
  OFPPC_NO_FWD = 1 << 5,
  OFPPC_NO_PACKET_IN = 1 << 6,

  OFPPC_OTHER_CONFIG_FLAGS = 0xffffff80
};

inline OFPPortConfigFlags operator|(OFPPortConfigFlags lhs,
                                    OFPPortConfigFlags rhs) {
  return static_cast<OFPPortConfigFlags>(static_cast<UInt32>(lhs) | rhs);
}

// Current state of the physical port.  These are not configurable from
// the controller.
enum OFPPortStateFlags : UInt32 {
  OFPPS_NONE = 0,
  OFPPS_LINK_DOWN = 1 << 0,
  OFPPS_BLOCKED = 1 << 1,
  OFPPS_LIVE = 1 << 2,

  OFPPS_STP_LISTEN = 0 << 8,  // Version 1.0 only
  OFPPS_STP_LEARN = 1 << 8,
  OFPPS_STP_FORWARD = 2 << 8,
  OFPPS_STP_BLOCK = 3 << 8,
  OFPPS_STP_MASK = 3 << 8,

  OFPPS_OTHER_STATE_FLAGS = 0xfffffff8,
  OFPPS_OTHER_STATE_FLAGS_V1 = 0xfffffcf8
};

inline OFPPortStateFlags operator|(OFPPortStateFlags lhs,
                                   OFPPortStateFlags rhs) {
  return static_cast<OFPPortStateFlags>(static_cast<UInt32>(lhs) | rhs);
}

// Special Port numbers.
enum OFPPortNo : UInt32 {
  // Maximum number of physical and logical switch ports.
  OFPP_MAX = 0xffffff00,

  // Reserved OpenFlow Ports (fake output "ports").
  OFPP_IN_PORT = 0xfffffff8,
  OFPP_TABLE = 0xfffffff9,
  OFPP_NORMAL = 0xfffffffa,
  OFPP_FLOOD = 0xfffffffb,
  OFPP_ALL = 0xfffffffc,
  OFPP_CONTROLLER = 0xfffffffd,
  OFPP_LOCAL = 0xfffffffe,
  OFPP_ANY = 0xffffffff,
  OFPP_NONE = 0xffffffff,  // Version 1.0 only
};

// Features of ports available in a datapath.
enum OFPPortFeaturesFlags : UInt32 {
  OFPPF_NONE = 0,
  OFPPF_10MB_HD = 1 << 0,
  OFPPF_10MB_FD = 1 << 1,
  OFPPF_100MB_HD = 1 << 2,
  OFPPF_100MB_FD = 1 << 3,
  OFPPF_1GB_HD = 1 << 4,
  OFPPF_1GB_FD = 1 << 5,
  OFPPF_10GB_FD = 1 << 6,
  OFPPF_40GB_FD = 1 << 7,
  OFPPF_100GB_FD = 1 << 8,
  OFPPF_1TB_FD = 1 << 9,
  OFPPF_OTHER = 1 << 10,
  OFPPF_COPPER = 1 << 11,
  OFPPF_FIBER = 1 << 12,
  OFPPF_AUTONEG = 1 << 13,
  OFPPF_PAUSE = 1 << 14,
  OFPPF_PAUSE_ASYM = 1 << 15,

  OFPPF_OTHER_FEATURES_FLAGS = 0xFFFF0000
};

inline OFPPortFeaturesFlags operator|(OFPPortFeaturesFlags lhs,
                                      OFPPortFeaturesFlags rhs) {
  return static_cast<OFPPortFeaturesFlags>(static_cast<UInt32>(lhs) | rhs);
}

OFPPortFeaturesFlags OFPPortFeaturesFlagsConvertToV1(UInt32 features);
OFPPortFeaturesFlags OFPPortFeaturesFlagsConvertFromV1(UInt32 features);

enum OFPOpticalPortFeaturesFlags : UInt32 {
  OFPOPF_NONE = 0,
  OFPOPF_RX_TUNE = 1 << 0,
  OFPOPF_TX_TUNE = 1 << 1,
  OFPOPF_TX_PWR = 1 << 2,
  OFPOPF_USE_FREQ  = 1 << 3,

  OFPOPF_OTHER_FEATURES_FLAGS = 0xFFFFFFF0
};

inline OFPOpticalPortFeaturesFlags operator|(OFPOpticalPortFeaturesFlags lhs,
                                      OFPOpticalPortFeaturesFlags rhs) {
  return static_cast<OFPOpticalPortFeaturesFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPPortStatusReason : UInt8 {
  OFPPR_ADD = 0,
  OFPPR_DELETE = 1,
  OFPPR_MODIFY = 2,
};

enum OFPPortStatusFlags : UInt32 {
  OFPPRF_NONE = 0,
  OFPPRF_ADD = 1 << 0,
  OFPPRF_DELETE = 1 << 1,
  OFPPRF_MODIFY = 1 << 2,
  OFPPRF_OTHER_PORTSTATUS_FLAGS = 0xfffffff8
};

inline OFPPortStatusFlags operator|(OFPPortStatusFlags lhs,
                                    OFPPortStatusFlags rhs) {
  return static_cast<OFPPortStatusFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPControllerMaxLen : UInt16 {
  OFPCML_MAX = 0xffe5,
  OFPCML_NO_BUFFER = 0xffff
};

// Table numbering. Tables can use any number up to OFPT_MAX.
enum OFPTableNo : UInt8 {
  // Last usable table number.
  OFPTT_MAX = 0xfe,
  // Fake tables.
  OFPTT_ALL = 0xff
};

// Group numbering. Groups can use any number up to OFPG_MAX.
enum OFPGroupNo : UInt32 {
  // Last usable group number.
  OFPG_MAX = 0xffffff00,
  // Fake groups.
  OFPG_ALL = 0xfffffffc,  // Represents all groups for group delete commands.
  OFPG_ANY = 0xffffffff
  // Wildcard group used only for flow stats requests. Selects all flows
  // regardless of group (including flows with no group).
};

enum OFPMultipartType : UInt16 {
  // Description of this OpenFlow switch.
  // The request body is empty.
  // The reply body is struct ofp_desc.
  OFPMP_DESC = 0,
  // Individual flow statistics.
  // The request body is struct ofp_flow_stats_request.
  // The reply body is an array of struct ofp_flow_stats.
  OFPMP_FLOW = 1,
  // Aggregate flow statistics.
  // The request body is struct ofp_aggregate_stats_request.
  // The reply body is struct ofp_aggregate_stats_reply.
  OFPMP_AGGREGATE = 2,
  // Flow table statistics.
  // The request body is empty.
  // The reply body is an array of struct ofp_table_stats.
  OFPMP_TABLE = 3,
  // Port statistics.
  // The request body is struct ofp_port_stats_request.
  // The reply body is an array of struct ofp_port_stats.
  OFPMP_PORT_STATS = 4,
  // Queue statistics for a port
  // The request body is struct ofp_queue_stats_request.
  // The reply body is an array of struct ofp_queue_stats
  OFPMP_QUEUE = 5,
  // Group counter statistics.
  // The request body is struct ofp_group_stats_request.
  // The reply is an array of struct ofp_group_stats.
  OFPMP_GROUP = 6,
  // Group description.
  // The request body is empty.
  // The reply body is an array of struct ofp_group_desc.
  OFPMP_GROUP_DESC = 7,
  // Group features.
  // The request body is empty.
  // The reply body is struct ofp_group_features.
  OFPMP_GROUP_FEATURES = 8,
  // Meter statistics.
  // The request body is struct ofp_meter_multipart_requests.
  // The reply body is an array of struct ofp_meter_stats.
  OFPMP_METER = 9,
  // Meter configuration.
  // The request body is struct ofp_meter_multipart_requests.
  // The reply body is an array of struct ofp_meter_config.
  OFPMP_METER_CONFIG = 10,
  // Meter features.
  // The request body is empty.
  // The reply body is struct ofp_meter_features.
  OFPMP_METER_FEATURES = 11,
  // Table features.
  // The request body is either empty or contains an array of
  // struct ofp_table_features containing the controller's
  // desired view of the switch. If the switch is unable to
  // set the specified view an error is returned.
  // The reply body is an array of struct ofp_table_features.
  OFPMP_TABLE_FEATURES = 12,
  // Port description.
  // The request body is empty.
  // The reply body is an array of struct ofp_port.
  OFPMP_PORT_DESC = 13,
  OFPMP_TABLE_DESC = 14,
  OFPMP_QUEUE_DESC = 15,
  OFPMP_FLOW_MONITOR = 16,

  OFPMP_EXPERIMENTER = 0xffff
};

std::ostream &operator<<(std::ostream &os, OFPMultipartType type);

enum OFPInstructionType : UInt16 {
  OFPIT_NONE = 0,
  OFPIT_GOTO_TABLE = 1,
  OFPIT_WRITE_METADATA = 2,
  OFPIT_WRITE_ACTIONS = 3,
  OFPIT_APPLY_ACTIONS = 4,
  OFPIT_CLEAR_ACTIONS = 5,
  OFPIT_METER = 6,         // OFPIT_DEPRECATED in v1.5
  OFPIT_STAT_TRIGGER = 7,  // Added in v1.5
  OFPIT_EXPERIMENTER = 0xFFFF
};

enum OFPActionType : UInt16 {
  OFPAT_OUTPUT = 0,
  OFPAT_COPY_TTL_OUT = 11,
  OFPAT_COPY_TTL_IN = 12,
  OFPAT_SET_MPLS_TTL = 15,
  OFPAT_DEC_MPLS_TTL = 16,
  OFPAT_PUSH_VLAN = 17,
  OFPAT_POP_VLAN = 18,
  OFPAT_PUSH_MPLS = 19,
  OFPAT_POP_MPLS = 20,
  OFPAT_SET_QUEUE = 21,
  OFPAT_GROUP = 22,
  OFPAT_SET_NW_TTL = 23,
  OFPAT_DEC_NW_TTL = 24,
  OFPAT_SET_FIELD = 25,  // 32 possible lengths: 8, 16, 24, 32, 40, ..., 256
  OFPAT_PUSH_PBB = 26,   // New in v4
  OFPAT_POP_PBB = 27,    // New in v4

  // "Fake" Version 1 actions.
  OFPAT_STRIP_VLAN_V1 = 0x7FF1,
  OFPAT_ENQUEUE_V1 = 0x7FF2,
  OFPAT_EXPERIMENTER = 0xFFFF  // possible lengths: 8, 16, 24, 32, 40, ...
};

enum OFPPacketInReason : UInt8 {
  OFPR_TABLE_MISS = 0,    // 1.4 name; instead of OFPR_NO_MATCH
  OFPR_APPLY_ACTION = 1,  // 1.4 name; instead of OFPR_ACTION
  OFPR_INVALID_TTL = 2,   // added in 1.2
  OFPR_ACTION_SET = 3,    // added in 1.4
  OFPR_GROUP = 4,         // added in 1.4
  OFPR_PACKET_OUT = 5,    // added in 1.4
};

enum OFPPacketInFlags : UInt32 {
  OFPRF_NONE = 0,
  OFPRF_TABLE_MISS = 1 << 0,
  OFPRF_APPLY_ACTION = 1 << 1,
  OFPRF_INVALID_TTL = 1 << 2,
  OFPRF_ACTION_SET = 1 << 3,
  OFPRF_GROUP = 1 << 4,
  OFPRF_PACKET_OUT = 1 << 5,
  OFPRF_OTHER_PACKET_IN_FLAGS = 0xffffffC0
};

inline OFPPacketInFlags operator|(OFPPacketInFlags lhs, OFPPacketInFlags rhs) {
  return static_cast<OFPPacketInFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPFlowModCommand : UInt8 {
  OFPFC_ADD = 0,
  OFPFC_MODIFY = 1,
  OFPFC_MODIFY_STRICT = 2,
  OFPFC_DELETE = 3,
  OFPFC_DELETE_STRICT = 4
};

enum OFPFlowRemovedReason : UInt8 {
  OFPRR_IDLE_TIMEOUT = 0,
  OFPRR_HARD_TIMEOUT = 1,
  OFPRR_DELETE = 2,
  OFPRR_GROUP_DELETE = 3,
  OFPRR_METER_DELETE = 4,
  OFPRR_EVICTION = 5,
};

enum OFPFlowRemovedFlags : UInt32 {
  OFPRRF_NONE = 0,
  OFPRRF_IDLE_TIMEOUT = 1 << 0,
  OFPRRF_HARD_TIMEOUT = 1 << 1,
  OFPRRF_DELETE = 1 << 2,
  OFPRRF_GROUP_DELETE = 1 << 3,
  OFPRRF_METER_DELETE = 1 << 4,
  OFPRRF_EVICTION = 1 << 5,

  OFPRRF_OTHER_FLOWREMOVED_FLAGS = 0xffffffC0
};

inline OFPFlowRemovedFlags operator|(OFPFlowRemovedFlags lhs,
                                     OFPFlowRemovedFlags rhs) {
  return static_cast<OFPFlowRemovedFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPFlowModFlags : UInt16 {
  OFPFF_NONE = 0,

  OFPFF_SEND_FLOW_REM = 1 << 0,
  OFPFF_CHECK_OVERLAP = 1 << 1,
  OFPFF_RESET_COUNTS = 1 << 2,
  OFPFF_NO_PKT_COUNTS = 1 << 3,
  OFPFF_NO_BYT_COUNTS = 1 << 4,

  // Not supported: 1.0 OFPFF_EMERG

  OFPFF_OTHER_FLAGS = 0xFFE0
};

inline OFPFlowModFlags operator|(OFPFlowModFlags lhs, OFPFlowModFlags rhs) {
  return static_cast<OFPFlowModFlags>(static_cast<UInt16>(lhs) | rhs);
}

enum OFPActionTypeFlags : UInt32 {
  OFPATF_NONE = 0,

  OFPATF_OUTPUT = 1 << 0,
  OFPATF_SET_VLAN_VID = 1 << 1,
  OFPATF_SET_VLAN_PCP = 1 << 2,
  OFPATF_SET_DL_SRC = 1 << 3,
  OFPATF_SET_DL_DST = 1 << 4,
  OFPATF_SET_NW_SRC = 1 << 5,
  OFPATF_SET_NW_DST = 1 << 6,
  OFPATF_SET_NW_TOS = 1 << 7,
  OFPATF_SET_NW_ECN = 1 << 8,
  OFPATF_SET_TP_SRC = 1 << 9,
  OFPATF_SET_TP_DST = 1 << 10,
  OFPATF_COPY_TTL_OUT = 1 << 11,
  OFPATF_COPY_TTL_IN = 1 << 12,
  OFPATF_SET_MPLS_LABEL = 1 << 13,
  OFPATF_SET_MPLS_TC = 1 << 14,
  OFPATF_SET_MPLS_TTL = 1 << 15,
  OFPATF_DEC_MPLS_TTL = 1 << 16,
  OFPATF_PUSH_VLAN = 1 << 17,
  OFPATF_POP_VLAN = 1 << 18,
  OFPATF_PUSH_MPLS = 1 << 19,
  OFPATF_POP_MPLS = 1 << 20,
  OFPATF_SET_QUEUE = 1 << 21,
  OFPATF_GROUP = 1 << 22,
  OFPATF_SET_NW_TTL = 1 << 23,
  OFPATF_DEC_NW_TTL = 1 << 24,
  OFPATF_SET_FIELD = 1 << 25,
  OFPATF_PUSH_PBB = 1 << 26,
  OFPATF_POP_PBB = 1 << 27,

  // Special values for version 1.0 actions
  OFPATF_STRIP_VLAN_V1 = 1U << 30,
  OFPATF_ENQUEUE_V1 = 1U << 31,

  OFPATF_OTHER_FLAGS = 0x30000000
};

inline OFPActionTypeFlags operator|(OFPActionTypeFlags lhs,
                                    OFPActionTypeFlags rhs) {
  return static_cast<OFPActionTypeFlags>(static_cast<UInt32>(lhs) | rhs);
}

OFPActionTypeFlags OFPActionTypeFlagsConvertToV1(UInt32 actions);
OFPActionTypeFlags OFPActionTypeFlagsConvertFromV1(UInt32 actions);

enum OFPCapabilitiesFlags : UInt32 {
  OFPC_NONE = 0,

  OFPC_FLOW_STATS = 1 << 0,
  OFPC_TABLE_STATS = 1 << 1,
  OFPC_PORT_STATS = 1 << 2,
  OFPC_GROUP_STATS = 1 << 3,
  // OFPC_RESERVED = 1 << 4
  OFPC_IP_REASM = 1 << 5,
  OFPC_QUEUE_STATS = 1 << 6,
  OFPC_ARP_MATCH_IP = 1 << 7,
  OFPC_PORT_BLOCKED = 1 << 8,

  OFPC_STP = 1U << 31,  // Special value for 1.0

  OFPC_OTHER_CAPABILITIES_FLAGS = 0x7FFFFE10
};

// TODO(bfish): Use template enabled only for enum/UInt32 types.
inline OFPCapabilitiesFlags operator|(OFPCapabilitiesFlags lhs,
                                      OFPCapabilitiesFlags rhs) {
  return static_cast<OFPCapabilitiesFlags>(static_cast<UInt32>(lhs) | rhs);
}

OFPCapabilitiesFlags OFPCapabilitiesFlagsConvertToV1(UInt32 capabilities);
OFPCapabilitiesFlags OFPCapabilitiesFlagsConvertFromV1(UInt32 capabilities);

enum OFPConfigFlags : UInt16 {
  OFPC_FRAG_NORMAL = 0,
  OFPC_FRAG_DROP = 1,
  OFPC_FRAG_REASM = 2,
  OFPC_FRAG_MASK = 3,

  OFPC_OTHER_CONFIG_FLAGS = 0xFFFC
};

inline OFPConfigFlags operator|(OFPConfigFlags lhs, OFPConfigFlags rhs) {
  return static_cast<OFPConfigFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPQueueProperty : UInt16 {
  OFPQT_MIN_RATE = 1,
  OFPQT_MAX_RATE = 2,
  OFPQT_UNUSED_MIN = 3,  // min unused property id
  OFPQT_EXPERIMENTER = 0xffff
};

enum OFPMeterBandType : UInt16 {
  OFPMBT_NONE = 0,
  OFPMBT_DROP = 1,
  OFPMBT_DSCP_REMARK = 2,
  OFPMBT_EXPERIMENTER = 0xffff
};

enum OFPTableFeatureProperty : UInt16 {
  OFPTFPT_INSTRUCTIONS = 0,
  OFPTFPT_INSTRUCTIONS_MISS = 1,
  OFPTFPT_NEXT_TABLES = 2,
  OFPTFPT_NEXT_TABLES_MISS = 3,
  OFPTFPT_WRITE_ACTIONS = 4,
  OFPTFPT_WRITE_ACTIONS_MISS = 5,
  OFPTFPT_APPLY_ACTIONS = 6,
  OFPTFPT_APPLY_ACTIONS_MISS = 7,
  OFPTFPT_MATCH = 8,
  OFPTFPT_UNUSED_9 = 9,
  OFPTFPT_WILDCARDS = 10,
  OFPTFPT_UNUSED_11 = 11,
  OFPTFPT_WRITE_SETFIELD = 12,
  OFPTFPT_WRITE_SETFIELD_MISS = 13,
  OFPTFPT_APPLY_SETFIELD = 14,
  OFPTFPT_APPLY_SETFIELD_MISS = 15,
  OFPTFPT_UNUSED_MIN = 16,  // min unused property id
  OFPTFPT_EXPERIMENTER = 0xFFFE,
  OFPTFPT_EXPERIMENTER_MISS = 0xFFFF
};

enum OFPControllerRole : UInt32 {
  OFPCR_ROLE_NOCHANGE = 0,
  OFPCR_ROLE_EQUAL = 1,
  OFPCR_ROLE_MASTER = 2,
  OFPCR_ROLE_SLAVE = 3,
};

enum OFPRoleStatusProperty : UInt16 {
  OFPRPT_UNUSED_MIN = 0,  // min unused property id
  OFPRPT_EXPERIMENTER = 0xFFFF,
};

enum OFPBundleProperty : UInt16 {
  OFPBPT_UNUSED_MIN = 0,  // min unused property id
  OFPBPT_EXPERIMENTER = 0xFFFF,
};

enum OFPPortProperty : UInt16 {
  OFPPDPT_ETHERNET = 0,
  OFPPDPT_OPTICAL = 1,
  OFPPDPT_UNUSED_MIN = 2,  // min unused property id
  OFPPDPT_EXPERIMENTER = 0xFFFF,
};

enum OFPMultipartFlags : UInt16 {
  OFPMPF_NONE = 0,
  OFPMPF_MORE = 1 << 0,

  OFPMPF_OTHER_MULTIPART_FLAGS = 0xFFFE
};

inline OFPMultipartFlags operator|(OFPMultipartFlags lhs,
                                   OFPMultipartFlags rhs) {
  return static_cast<OFPMultipartFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPMeterModCommand : UInt16 {
  OFPMC_ADD = 0,
  OFPMC_MODIFY = 1,
  OFPMC_DELETE = 2
};

enum OFPMeterConfigFlags : UInt16 {
  OFPMF_NONE = 0,

  OFPMF_KBPS = 1 << 0,
  OFPMF_PKTPS = 1 << 1,
  OFPMF_BURST = 1 << 2,
  OFPMF_STATS = 1 << 3,

  OFPMF_OTHER_METER_CONFIG_FLAGS = 0xFFF0
};

inline OFPMeterConfigFlags operator|(OFPMeterConfigFlags lhs,
                                     OFPMeterConfigFlags rhs) {
  return static_cast<OFPMeterConfigFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPIPv6ExtHdrFlags : UInt16 {
  OFPIEH_NONEXT = 1 << 0,
  OFPIEH_ESP = 1 << 1,
  OFPIEH_AUTH = 1 << 2,
  OFPIEH_DEST = 1 << 3,
  OFPIEH_FRAG = 1 << 4,
  OFPIEH_ROUTER = 1 << 5,
  OFPIEH_HOP = 1 << 6,
  OFPIEH_UNREP = 1 << 7,
  OFPIEH_UNSEQ = 1 << 8,
};

enum OFPFlowUpdateEvent : UInt16 {
  OFPFME_INITIAL = 0,
  OFPFME_ADDED = 1,
  OFPFME_REMOVED = 2,
  OFPFME_MODIFIED = 3,
  OFPFME_ABBREV = 4,
  OFPFME_PAUSED = 5,
  OFPFME_RESUMED = 6,
};

}  // namespace ofp

#endif  // OFP_CONSTANTS_H_
