// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CONSTANTS_H_
#define OFP_CONSTANTS_H_

#include "ofp/constants_deprecated.h"
#include "ofp/constants_error.h"
#include "ofp/protocoliteratortype.h"
#include "ofp/types.h"

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
  DATALINK_BDDP = 0x8942,
  DATALINK_8021Q = 0x8100
};

enum : UInt8 {
  ICMPV6_TYPE_NEIGHBOR_SOLICIT = 135,
  ICMPV6_TYPE_NEIGHBOR_ADVERTISE = 136,
  ICMPV6_OPTION_SLL = 1,
  ICMPV6_OPTION_TLL = 2,
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
  OFP_VERSION_6 = 0x06,

  // The last supported version.
  OFP_VERSION_LAST = OFP_VERSION_6,
  OFP_VERSION_MAX_ALLOWED = 0x09
};

enum {
  OFP_DEFAULT_PORT = 6653,

  // Maximum size of an OpenFlow message.
  OFP_MAX_SIZE = 65535
};

/// Retrieve default OpenFlow port, which may be overidden by an environment
/// variable.
UInt16 OFPGetDefaultPort();

enum : UInt16 {
  OFPVID_PRESENT = 0x1000,
  OFPVID_NONE = 0x0000,
  OFPVID_MASK = 0x0fff
};

// NXM_NX_IP_FRAG
enum : UInt8 {
  NXM_FRAG_TYPE_NONE = 0x00,
  NXM_FRAG_TYPE_ANY = 0x01,
  NXM_FRAG_TYPE_LATER = 0x02,
};

enum { OFPMT_STANDARD = 0, OFPMT_OXM = 1 };

enum OFPType : UInt8 {
  OFPT_HELLO = 0,
  OFPT_ERROR = 1,
  OFPT_ECHO_REQUEST = 2,
  OFPT_ECHO_REPLY = 3,
  OFPT_EXPERIMENTER = 4,  // Use instead of OFPT_VENDOR
  OFPT_FEATURES_REQUEST = 5,
  OFPT_FEATURES_REPLY = 6,
  OFPT_GET_CONFIG_REQUEST = 7,
  OFPT_GET_CONFIG_REPLY = 8,
  OFPT_SET_CONFIG = 9,
  OFPT_PACKET_IN = 10,
  OFPT_FLOW_REMOVED = 11,
  OFPT_PORT_STATUS = 12,
  OFPT_PACKET_OUT = 13,
  OFPT_FLOW_MOD = 14,
  OFPT_GROUP_MOD = 15,  // Min: 1.1
  OFPT_PORT_MOD = 16,
  OFPT_TABLE_MOD = 17,          // Min: 1.1
  OFPT_MULTIPART_REQUEST = 18,  // Use instead of OFPT_STATS_REQUEST
  OFPT_MULTIPART_REPLY = 19,    // Use instead of OFPT_STATS_REPLY
  OFPT_BARRIER_REQUEST = 20,
  OFPT_BARRIER_REPLY = 21,
  OFPT_QUEUE_GET_CONFIG_REQUEST = 22,
  OFPT_QUEUE_GET_CONFIG_REPLY = 23,
  OFPT_ROLE_REQUEST = 24,        // Min: 1.2
  OFPT_ROLE_REPLY = 25,          // Min: 1.2
  OFPT_GET_ASYNC_REQUEST = 26,   // Min: 1.3
  OFPT_GET_ASYNC_REPLY = 27,     // Min: 1.3
  OFPT_SET_ASYNC = 28,           // Min: 1.3
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
  OFPT_RAW_MESSAGE = 0xFA
};

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, OFPType type);

enum OFPBufferNo : UInt32 { OFP_NO_BUFFER = 0xFFFFFFFFUL };

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
  OFPOPF_USE_FREQ = 1 << 3,

  OFPOPF_OTHER_FEATURES_FLAGS = 0xFFFFFFF0
};

inline OFPOpticalPortFeaturesFlags operator|(OFPOpticalPortFeaturesFlags lhs,
                                             OFPOpticalPortFeaturesFlags rhs) {
  return static_cast<OFPOpticalPortFeaturesFlags>(static_cast<UInt32>(lhs) |
                                                  rhs);
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

enum OFPTableNo : UInt8 { OFPTT_MAX = 0xfe, OFPTT_ALL = 0xff };

enum OFPGroupNo : UInt32 {
  OFPG_MAX = 0xffffff00,
  OFPG_ALL = 0xfffffffc,
  OFPG_ANY = 0xffffffff
};

enum OFPQueueNo : UInt32 { OFPQ_ALL = 0xffffffff };

enum OFPMeterNo : UInt32 {
  OFPM_MAX = 0xffff0000,
  OFPM_SLOWPATH = 0xfffffffd,
  OFPM_CONTROLLER = 0xfffffffe,
  OFPM_ALL = 0xffffffff
};

enum OFPMultipartType : UInt16 {
  OFPMP_DESC = 0,
  OFPMP_FLOW_DESC = 1,
  OFPMP_AGGREGATE_STATS = 2,
  OFPMP_TABLE_STATS = 3,
  OFPMP_PORT_STATS = 4,
  OFPMP_QUEUE_STATS = 5,
  OFPMP_GROUP_STATS = 6,
  OFPMP_GROUP_DESC = 7,
  OFPMP_GROUP_FEATURES = 8,
  OFPMP_METER_STATS = 9,
  OFPMP_METER_CONFIG = 10,
  OFPMP_METER_FEATURES = 11,
  OFPMP_TABLE_FEATURES = 12,
  OFPMP_PORT_DESC = 13,
  OFPMP_TABLE_DESC = 14,
  OFPMP_QUEUE_DESC = 15,
  OFPMP_FLOW_MONITOR = 16,

  OFPMP_UNSUPPORTED = 0xf9f9,
  OFPMP_EXPERIMENTER = 0xffff
};

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, OFPMultipartType type);

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
  OFPAT_STRIP_VLAN_V1 = 0x7FF1,  // FIXME(bfish): still necessary?
  OFPAT_ENQUEUE_V1 = 0x7FF2,     // FIXME(bfish): still necessary?
  OFPAT_EXPERIMENTER = 0xFFFF    // possible lengths: 8, 16, 24, 32, 40, ...
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

enum OFPRoleStatusReason : UInt8 {
  OFPCRR_MASTER_REQUEST = 0,
  OFPCRR_CONFIG = 1,
  OFPCRR_EXPERIMENTER = 2,
};

enum OFPRoleStatusFlags : UInt32 {
  OFPCRRF_MASTER_REQUEST = 1 << 0,
  OFPCRRF_CONFIG = 1 << 1,
  OFPCRRF_EXPERIMENTER = 1 << 2,

  OFPCRRF_OTHER_ROLESTATUS_FLAGS = 0xFFFFFFF8
};

inline OFPRoleStatusFlags operator|(OFPRoleStatusFlags lhs,
                                    OFPRoleStatusFlags rhs) {
  return static_cast<OFPRoleStatusFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPTableStatusReason : UInt8 {
  OFPTR_VACANCY_DOWN = 3,
  OFPTR_VACANCY_UP = 4,
};

enum OFPTableStatusFlags : UInt32 {
  OFPTRF_VACANCY_DOWN = 1 << 3,
  OFPTRF_VACANCY_UP = 1 << 4,

  OFPTRF_OTHER_TABLESTATUS_FLAGS = 0xFFFFFFE7
};

inline OFPTableStatusFlags operator|(OFPTableStatusFlags lhs,
                                     OFPTableStatusFlags rhs) {
  return static_cast<OFPTableStatusFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPRequestForwardReason : UInt8 {
  OFPRFR_GROUP_MOD = 0,
  OFPRFR_METER_MOD = 1,
};

enum OFPRequestForwardFlags : UInt32 {
  OFPRFRF_GROUP_MOD = 1 << 0,
  OFPRFRF_METER_MOD = 1 << 1,

  OFPRFRF_OTHER_REQUESTFORWARD_FLAGS = 0xFFFFFFFC
};

inline OFPRequestForwardFlags operator|(OFPRequestForwardFlags lhs,
                                        OFPRequestForwardFlags rhs) {
  return static_cast<OFPRequestForwardFlags>(static_cast<UInt32>(lhs) | rhs);
}

// Pre-1.4 Queue properties; new ones are OFPQueueDescProperty.
enum OFPQueueProperty : UInt16 {
  OFPQT_NONE = 0,  // used in 1.0, 1.1 to indicate default
  OFPQT_MIN_RATE = 1,
  OFPQT_MAX_RATE = 2,
  OFPQT_UNUSED_MIN = 3,  // min unused property id
  OFPQT_EXPERIMENTER = 0xffff
};

enum OFPQueueDescProperty : UInt16 {
  OFPQDPT_MIN_RATE = 1,
  OFPQDPT_MAX_RATE = 2,
  OFPQDPT_UNUSED_MIN = 3,
  OFPQDPT_EXPERIMENTER = 0xffff
};

enum OFPMeterBandType : UInt16 {
  OFPMBT_NONE = 0,
  OFPMBT_DROP = 1,
  OFPMBT_DSCP_REMARK = 2,
  OFPMBT_EXPERIMENTER = 0xffff
};

enum OFPMeterBandFlags : UInt32 {
  OFPMBTF_NONE = 1 << 0,
  OFPMBTF_DROP = 1 << 1,
  OFPMBTF_DSCP_REMARK = 1 << 2,

  OFPMBTF_OTHER_METERBAND_FLAGS = 0xFFFFFFF8
};

inline OFPMeterBandFlags operator|(OFPMeterBandFlags lhs,
                                   OFPMeterBandFlags rhs) {
  return static_cast<OFPMeterBandFlags>(static_cast<UInt32>(lhs) | rhs);
}

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

enum OFPTableModProperty : UInt16 {
  OFPTMPT_UNUSED_MAX = 1,  // max unused property id
  OFPTMPT_EVICTION = 2,
  OFPTMPT_VACANCY = 3,
  OFPTMPT_UNUSED_MIN = 4,  // min unused property id
  OFPTMPT_EXPERIMENTER = 0xFFFF,
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

enum OFPPortModProperty : UInt16 {
  OFPPMPT_ETHERNET = 0,
  OFPPMPT_OPTICAL = 1,
  OFPPMPT_UNUSED_MIN = 2,  // min unused property id
  OFPPMPT_EXPERIMENTER = 0xFFFF,
};

enum OFPPortStatsProperty : UInt16 {
  OFPPSPT_ETHERNET = 0,
  OFPPSPT_OPTICAL = 1,
  OFPPSPT_UNUSED_MIN = 2,  // min unused property id
  OFPPSPT_EXPERIMENTER = 0xFFFF,
};

enum OFPAsyncConfigProperty : UInt16 {
  OFPACPT_PACKET_IN_SLAVE = 0,
  OFPACPT_PACKET_IN_MASTER = 1,
  OFPACPT_PORT_STATUS_SLAVE = 2,
  OFPACPT_PORT_STATUS_MASTER = 3,
  OFPACPT_FLOW_REMOVED_SLAVE = 4,
  OFPACPT_FLOW_REMOVED_MASTER = 5,
  OFPACPT_ROLE_STATUS_SLAVE = 6,
  OFPACPT_ROLE_STATUS_MASTER = 7,
  OFPACPT_TABLE_STATUS_SLAVE = 8,
  OFPACPT_TABLE_STATUS_MASTER = 9,
  OFPACPT_REQUESTFORWARD_SLAVE = 10,
  OFPACPT_REQUESTFORWARD_MASTER = 11,
  OFPACPT_UNUSED_MIN = 12,  // min unused property id
  OFPACPT_EXPERIMENTER_SLAVE = 0xFFFE,
  OFPACPT_EXPERIMENTER_MASTER = 0xFFFF,
};

enum OFPExperimenterPropertyType : UInt16 {
  OFP_EXPERIMENTER_PROPERTY_TYPE = 0xFFFF
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

enum OFPMessageFlags : UInt32 {
  // Lower 16 bits are OFPMultipartFlags. The rest are internal message flags.
  OFP_DEFAULT_MESSAGE_FLAGS = 0,
  OFP_MORE = static_cast<UInt32>(OFPMPF_MORE),
  OFP_NO_FLUSH = 1 << 16,
  OFP_NO_ALERT = 1 << 17,
  OFP_REPLIED = 1 << 18,

  OFP_OTHER_MESSAGE_FLAGS = 0xFFF8FFFE
};
static_assert(0xFFFFFFFF == (OFP_OTHER_MESSAGE_FLAGS ^ OFP_MORE ^ OFP_NO_FLUSH ^
                             OFP_NO_ALERT ^ OFP_REPLIED),
              "Enum mis-define");

inline OFPMessageFlags operator|(OFPMessageFlags lhs, OFPMessageFlags rhs) {
  return static_cast<OFPMessageFlags>(static_cast<UInt32>(lhs) | rhs);
}

inline OFPMessageFlags operator|(OFPMessageFlags lhs, OFPMultipartFlags rhs) {
  return static_cast<OFPMessageFlags>(static_cast<UInt32>(lhs) | rhs);
}

inline OFPMultipartFlags toMultipartFlags(OFPMessageFlags flags) {
  return static_cast<OFPMultipartFlags>(flags & 0xffff);
}

enum OFPGroupModCommand : UInt16 {
  OFPGC_ADD = 0,
  OFPGC_MODIFY = 1,
  OFPGC_DELETE = 2
};

enum OFPGroupType : UInt8 {
  OFPGT_ALL = 0,
  OFPGT_SELECT = 1,
  OFPGT_INDIRECT = 2,
  OFPGT_FF = 3
};

enum OFPGroupTypeFlags : UInt32 {
  OFPGTF_NONE = 0,

  OFPGTF_ALL = 1 << 0,
  OFPGTF_SELECT = 1 << 1,
  OFPGTF_INDIRECT = 1 << 2,
  OFPGTF_FF = 1 << 3,

  OFPGTF_OTHER_GROUP_FLAGS = 0xFFFFFFF0
};

inline OFPGroupTypeFlags operator|(OFPGroupTypeFlags lhs,
                                   OFPGroupTypeFlags rhs) {
  return static_cast<OFPGroupTypeFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPGroupCapabilityFlags : UInt32 {
  OFPGFC_NONE = 0,

  OFPGFC_SELECT_WEIGHT = 1 << 0,
  OFPGFC_SELECT_LIVENESS = 1 << 1,
  OFPGFC_CHAINING = 1 << 2,
  OFPGFC_CHAINING_CHECKS = 1 << 3,

  OFPGFC_OTHER_GROUP_FLAGS = 0xFFFFFFF0
};

inline OFPGroupCapabilityFlags operator|(OFPGroupCapabilityFlags lhs,
                                         OFPGroupCapabilityFlags rhs) {
  return static_cast<OFPGroupCapabilityFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPMeterModCommand : UInt16 {
  OFPMC_ADD = 0,
  OFPMC_MODIFY = 1,
  OFPMC_DELETE = 2
};

enum OFPMeterConfigFlags : UInt16 {
  OFPMCF_NONE = 0,

  OFPMCF_KBPS = 1 << 0,
  OFPMCF_PKTPS = 1 << 1,
  OFPMCF_BURST = 1 << 2,
  OFPMCF_STATS = 1 << 3,

  OFPMCF_OTHER_METER_CONFIG_FLAGS = 0xFFF0
};

inline OFPMeterConfigFlags operator|(OFPMeterConfigFlags lhs,
                                     OFPMeterConfigFlags rhs) {
  return static_cast<OFPMeterConfigFlags>(static_cast<UInt32>(lhs) | rhs);
}

// OFPMeterFlags is the 32-bit equivalent of OFPMeterConfigFlags. It is needed
// because MeterMod assumes 16-bit and MeterFeatures assumes 32-bit.

enum OFPMeterFlags : UInt32 {
  OFPMF_NONE = 0,

  OFPMF_KBPS = 1 << 0,
  OFPMF_PKTPS = 1 << 1,
  OFPMF_BURST = 1 << 2,
  OFPMF_STATS = 1 << 3,

  OFPMF_OTHER_METER_FLAGS = 0xFFFFFFF0
};

inline OFPMeterFlags operator|(OFPMeterFlags lhs, OFPMeterFlags rhs) {
  return static_cast<OFPMeterFlags>(static_cast<UInt32>(lhs) | rhs);
}

enum OFPTableConfigFlags : UInt32 {
  OFPTC_TABLE_MISS_CONTROLLER = 0,  // 1.1, 1.2 only
  OFPTC_TABLE_MISS_CONTINUE = 1,    // 1.1, 1.2 only
  OFPTC_TABLE_MISS_DROP = 2,        // 1.1, 1.2 only
  OFPTC_TABLE_MISS_MASK = 3,        // 1.1, 1.2 only

  OFPTC_EVICTION = 1 << 2,        // 1.4+
  OFPTC_VACANCY_EVENTS = 1 << 3,  // 1.4+

  OFPTC_OTHER_TABLE_CONFIG_FLAGS = 0xFFFFFFF3,
  OFPTC_OTHER_TABLE_CONFIG_FLAGS_V2 = 0xFFFFFFFC,
  OFPTC_OTHER_TABLE_CONFIG_FLAGS_ALL = 0xFFFFFFFF
};

inline OFPTableConfigFlags operator|(OFPTableConfigFlags lhs,
                                     OFPTableConfigFlags rhs) {
  return static_cast<OFPTableConfigFlags>(static_cast<UInt32>(lhs) | rhs);
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

enum OFPFlowMonitorCommand : UInt8 {
  OFPFMC_ADD = 0,
  OFPFMC_MODIFY = 1,
  OFPFMC_DELETE = 2,
};

enum OFPFlowMonitorFlags : UInt16 {
  OFPFMF_INITIAL = 1 << 0,
  OFPFMF_ADD = 1 << 1,
  OFPFMF_REMOVED = 1 << 2,
  OFPFMF_MODIFY = 1 << 3,
  OFPFMF_INSTRUCTIONS = 1 << 4,
  OFPFMF_NO_ABBREV = 1 << 5,
  OFPFMF_ONLY_OWN = 1 << 6,

  OFPFMF_OTHER_FLOW_MONITOR_FLAGS = 0xffc0
};

inline OFPFlowMonitorFlags operator|(OFPFlowMonitorFlags lhs,
                                     OFPFlowMonitorFlags rhs) {
  return static_cast<OFPFlowMonitorFlags>(static_cast<UInt16>(lhs) | rhs);
}

enum OFPBundleCtrlType : UInt16 {
  OFPBCT_OPEN_REQUEST = 0,
  OFPBCT_OPEN_REPLY = 1,
  OFPBCT_CLOSE_REQUEST = 2,
  OFPBCT_CLOSE_REPLY = 3,
  OFPBCT_COMMIT_REQUEST = 4,
  OFPBCT_COMMIT_REPLY = 5,
  OFPBCT_DISCARD_REQUEST = 6,
  OFPBCT_DISCARD_REPLY = 7
};

enum OFPBundleFlags : UInt16 {
  OFPBF_ATOMIC = 1 << 0,
  OFPBF_ORDERED = 1 << 1,

  OFPBF_OTHER_BUNDLE_FLAGS = 0xfffc
};

inline OFPBundleFlags operator|(OFPBundleFlags lhs, OFPBundleFlags rhs) {
  return static_cast<OFPBundleFlags>(static_cast<UInt16>(lhs) | rhs);
}

}  // namespace ofp

#endif  // OFP_CONSTANTS_H_
