//  ===== ---- ofp/constants.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines all OpenFlow constants.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_CONSTANTS_H_
#define OFP_CONSTANTS_H_

#include "ofp/types.h"
#include "ofp/constants_deprecated.h"
#include "ofp/constants_error.h"

namespace ofp {

// Internet Protocol Transport Types
enum : UInt8 {
  PROTOCOL_ICMP = 1,
  PROTOCOL_TCP = 6,
  PROTOCOL_UDP = 17,
  PROTOCOL_SCTP = 132
};

enum : UInt16 {
  DATALINK_ARP = 0x0806,
  DATALINK_IPV4 = 0x0800,
  DATALINK_IPV6 = 0x86dd
};

enum : size_t {
  // Used by ProtocolIterator template class.
  PROTOCOL_ITERATOR_SIZE_FIXED = 0xffff,
  PROTOCOL_ITERATOR_SIZE_CONDITIONAL = 0xfffe
};

enum {
  OFP_MAX_PORT_NAME_LEN = 16,
  OFP_MAX_TABLE_NAME_LEN = 32
};

enum {
  OFP_VERSION_1 = 0x01,
  OFP_VERSION_2 = 0x02,
  OFP_VERSION_3 = 0x03,
  OFP_VERSION_4 = 0x04,
  OFP_VERSION_LAST = OFP_VERSION_4
};

enum {
  OFP_DEFAULT_PORT = 6633,

  // Maximum size of an OpenFlow message.
  OFP_MAX_SIZE = 65535
};

enum {
  OFP_NO_BUFFER = 0xFFFFFFFFUL
};

enum {
  OFPVID_PRESENT = 0x1000,
  OFPVID_NONE = 0x0000
};

enum {
  OFPMT_STANDARD = 0,
  OFPMT_OXM = 1
};

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
  OFPT_METER_MOD = 29,  // Min: 1.3

  /// Used internally.
  OFPT_LAST = OFPT_METER_MOD,
  OFPT_UNSUPPORTED = 0xF9
};

std::ostream &operator<<(std::ostream &os, OFPType type);

// Flags to indicate behavior of the physical port.  These flags are
// used in ofp_port to describe the current configuration.  They are
// used in the ofp_port_mod message to configure the port's behavior.

enum ofp_port_config {
  OFPPC_PORT_DOWN = 1 << 0,    // Port is administratively down.
  OFPPC_NO_RECV = 1 << 2,      // Drop all packets received by port.
  OFPPC_NO_FWD = 1 << 5,       // Drop packets forwarded to port.
  OFPPC_NO_PACKET_IN = 1 << 6  // Do not send packet-in msgs for port.
};

// Current state of the physical port.  These are not configurable from
// the controller.
enum ofp_port_state {
  OFPPS_LINK_DOWN = 1 << 0,  // No physical link present.
  OFPPS_BLOCKED = 1 << 1,    // Port is blocked
  OFPPS_LIVE = 1 << 2,       // Live for Fast Failover Group.
};

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
  OFPP_ANY = 0xffffffff
};

// Features of ports available in a datapath.
enum ofp_port_features {
  OFPPF_10MB_HD = 1 << 0,     // 10 Mb half-duplex rate support.
  OFPPF_10MB_FD = 1 << 1,     // 10 Mb full-duplex rate support.
  OFPPF_100MB_HD = 1 << 2,    // 100 Mb half-duplex rate support.
  OFPPF_100MB_FD = 1 << 3,    // 100 Mb full-duplex rate support.
  OFPPF_1GB_HD = 1 << 4,      // 1 Gb half-duplex rate support.
  OFPPF_1GB_FD = 1 << 5,      // 1 Gb full-duplex rate support.
  OFPPF_10GB_FD = 1 << 6,     // 10 Gb full-duplex rate support.
  OFPPF_40GB_FD = 1 << 7,     // 40 Gb full-duplex rate support.
  OFPPF_100GB_FD = 1 << 8,    // 100 Gb full-duplex rate support.
  OFPPF_1TB_FD = 1 << 9,      // 1 Tb full-duplex rate support.
  OFPPF_OTHER = 1 << 10,      // Other rate, not in the list.
  OFPPF_COPPER = 1 << 11,     // Copper medium.
  OFPPF_FIBER = 1 << 12,      // Fiber medium.
  OFPPF_AUTONEG = 1 << 13,    // Auto-negotiation.
  OFPPF_PAUSE = 1 << 14,      // Pause.
  OFPPF_PAUSE_ASYM = 1 << 15  // Asymmetric pause.
};

enum ofp_controller_max_len {
  OFPCML_MAX = 0xffe5,  // maximum max_len value which can be used to request a
                        // specific byte length.
  OFPCML_NO_BUFFER = 0xffff  // indicates that no buffering should be applied
                             // and the whole packet is to be sent to the
                             // controller.
};

// Hello elements types.
enum ofp_hello_elem_type {
  OFPHET_VERSIONBITMAP = 1,  // Bitmap of version supported.
};

// Table numbering. Tables can use any number up to OFPT_MAX.
enum ofp_table {
  // Last usable table number.
  OFPTT_MAX = 0xfe,
  // Fake tables.
  OFPTT_ALL = 0xff  // Wildcard table used for table config, flow stats and flow
                    // deletes.
};

// Group numbering. Groups can use any number up to OFPG_MAX.
enum ofp_group {
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
  // Experimenter extension.
  // The request and reply bodies begin with
  // struct ofp_experimenter_multipart_header.
  // The request and reply bodies are otherwise experimenter-defined.
  OFPMP_EXPERIMENTER = 0xffff
};

enum OFPInstructionType : UInt16 {
  OFPIT_GOTO_TABLE = 1,
  OFPIT_WRITE_METADATA = 2,
  OFPIT_WRITE_ACTIONS = 3,
  OFPIT_APPLY_ACTIONS = 4,
  OFPIT_CLEAR_ACTIONS = 5,
  OFPIT_METER = 6,
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
  OFPR_NO_MATCH = 0,     ///< No matching flow (table-miss flow entry)
  OFPR_ACTION = 1,       ///< Action explicitly output to controller
  OFPR_INVALID_TTL = 2,  ///< Packet has invalid TTL (added in v1.2)
};

enum OFPFlowModCommand : UInt8 {
  OFPFC_ADD = 0,
  OFPFC_MODIFY = 1,
  OFPFC_MODIFY_STRICT = 2,
  OFPFC_DELETE = 3,
  OFPFC_DELETE_STRICT = 4
};

enum OFPQueueProperty : UInt16 {
  OFPQT_MIN_RATE = 1,
  OFPQT_MAX_RATE = 2,
  OFPQT_UNUSED_MIN = 3,       // min unused property id
  OFPQT_EXPERIMENTER = 0xffff
};

enum OFPMeterBandType : UInt16 {
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
  OFPTFPT_UNUSED_MIN = 16,    // min unused property id
  OFPTFPT_EXPERIMENTER = 0xFFFE,
  OFPTFPT_EXPERIMENTER_MISS = 0xFFFF
};

}  // namespace ofp

#endif  // OFP_CONSTANTS_H_
