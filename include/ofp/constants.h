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

#ifndef OFP_CONSTANTS_H
#define OFP_CONSTANTS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

// Internet Protocol Transport Types
enum : UInt8 {
    ICMP = 1,
    TCP = 6,
    UDP = 17,
    SCTP = 132
};

enum {
    OFP_MAX_PORT_NAME_LEN = 16
};

enum {
    OFP_VERSION_1 = 0x01,
    OFP_VERSION_2 = 0x02,
    OFP_VERSION_3 = 0x03,
    OFP_VERSION_4 = 0x04,
    OFP_VERSION_LAST = OFP_VERSION_4
};

enum {
    OFP_DEFAULT_PORT = 6633
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

// Version:                    v1     v2     v3     v4
// ------------                --     --     --     --
// HELLO                        0      0      0      0
// ERROR                        1      1      1      1
// ECHO_REQUEST                 2      2      2      2
// ECHO_REPLY                   3      3      3      3
// VENDOR                       4 *    -      -      -
// EXPERIMENTER                 -      4      4      4
// FEATURES_REQUEST             5      5      5      5
// FEATURES_REPLY               6      6      6      6
// GET_CONFIG_REQUEST           7      7      7      7
// GET_CONFIG_REPLY             8      8      8      8
// SET_CONFIG                   9      9      9      9
// PACKET_IN                   10     10     10     10
// FLOW_REMOVED                11     11     11     11
// PORT_STATUS                 12     12     12     12
// PACKET_OUT                  13     13     13     13
// FLOW_MOD                    14 ^   14     14     14
// GROUP_MOD                    -     15     15     15
// PORT_MOD                    15 *   16     16     16
// TABLE_MOD                    -     17     17     17
// STATS_REQUEST               16 *   18     18      -
// STATS_REPLY                 17 *   19     19      -
// MULTIPART_REQUEST            -      -      -     18
// MULTIPART_REPLY              -      -      -     19
// BARRIER_REQUEST             18 *   20     20     20
// BARRIER_REPLY               19 *   21     21     21
// QUEUE_GET_CONFIG_REQUEST    20 *   22     22     22
// QUEUE_GET_CONFIG_REPLY      21 *   23     23     23
// ROLE_REQUEST                 -      -     24     24
// ROLE_REPLY                   -      -     25     25
// GET_ASYNC_REQUEST            -      -      -     26
// GET_ASYNC_REPLY              -      -      -     27
// SET_ASYNC                    -      -      -     28
// METER_MOD                    -      -      -     29

enum OFPType : UInt8 {
    OFPT_HELLO = 0,        // Symmetric message
    OFPT_ERROR = 1,        // Symmetric message
    OFPT_ECHO_REQUEST = 2, // Symmetric message
    OFPT_ECHO_REPLY = 3,   // Symmetric message
    OFPT_EXPERIMENTER = 4, // Symmetric message

    // Switch configuration messages.
    OFPT_FEATURES_REQUEST = 5,   // Controller -> switch message
    OFPT_FEATURES_REPLY = 6,     // Controller -> switch message
    OFPT_GET_CONFIG_REQUEST = 7, // Controller -> switch message
    OFPT_GET_CONFIG_REPLY = 8,   // Controller -> switch message
    OFPT_SET_CONFIG = 9,         // Controller -> switch message

    // Asynchronous messages.
    OFPT_PACKET_IN = 10,    // Switch -> controller message
    OFPT_FLOW_REMOVED = 11, // Switch -> controller message
    OFPT_PORT_STATUS = 12,  // Switch -> controller message

    // Controller command messages.
    OFPT_PACKET_OUT = 13, // Controller -> switch message
    OFPT_FLOW_MOD = 14,   // Controller -> switch message
    OFPT_GROUP_MOD = 15,  // Controller -> switch message
    OFPT_PORT_MOD = 16,   // Controller -> switch message
    OFPT_TABLE_MOD = 17,  // Controller -> switch message

    // Multipart messages.
    OFPT_MULTIPART_REQUEST = 18, // Controller -> switch message
    OFPT_MULTIPART_REPLY = 19,   // Controller -> switch message

    // Barrier messages.
    OFPT_BARRIER_REQUEST = 20, // Controller -> switch message
    OFPT_BARRIER_REPLY = 21,   // Controller -> switch message

    // Queue Configuration messages.
    OFPT_QUEUE_GET_CONFIG_REQUEST = 22, // Controller -> switch message
    OFPT_QUEUE_GET_CONFIG_REPLY = 23,   // Controller -> switch message

    // Controller role change request messages.
    OFPT_ROLE_REQUEST = 24, // Controller -> switch message
    OFPT_ROLE_REPLY = 25,   // Controller -> switch message

    // Asynchronous message configuration.
    OFPT_GET_ASYNC_REQUEST = 26, // Controller -> switch message
    OFPT_GET_ASYNC_REPLY = 27,   // Controller -> switch message
    OFPT_SET_ASYNC = 28,         // Controller -> switch message

    // Meters and rate limiters configuration messages.
    OFPT_METER_MOD = 29, // Controller -> switch message
    
    OFPT_LAST = OFPT_METER_MOD,
    /// Used internally to flag denote an improper version/type combination.
    OFPT_UNSUPPORTED = 0xF9
};

/* Flags to indicate behavior of the physical port.  These flags are
 * used in ofp_port to describe the current configuration.  They are
 * used in the ofp_port_mod message to configure the port's behavior.
 */
enum ofp_port_config {
    OFPPC_PORT_DOWN = 1 << 0,   /* Port is administratively down. */
    OFPPC_NO_RECV = 1 << 2,     /* Drop all packets received by port. */
    OFPPC_NO_FWD = 1 << 5,      /* Drop packets forwarded to port. */
    OFPPC_NO_PACKET_IN = 1 << 6 /* Do not send packet-in msgs for port. */
};

/* Current state of the physical port.  These are not configurable from
 * the controller.
 */
enum ofp_port_state {
    OFPPS_LINK_DOWN = 1 << 0, /* No physical link present. */
    OFPPS_BLOCKED = 1 << 1,   /* Port is blocked */
    OFPPS_LIVE = 1 << 2,      /* Live for Fast Failover Group. */
};

/* Port numbering. Ports are numbered starting from 1. */
enum ofp_port_no {
    /* Maximum number of physical and logical switch ports. */
    OFPP_MAX = 0xffffff00,
    /* Reserved OpenFlow Port (fake output "ports"). */

    OFPP_IN_PORT = 0xfffffff8, /* Send the packet out the input port.  This
                     reserved port must be explicitly used
                     in order to send back out of the input
                     port. */
    OFPP_TABLE = 0xfffffff9, /* Submit the packet to the first flow table
                     NB: This destination port can only be
                     used in packet-out messages. */
    OFPP_NORMAL = 0xfffffffa, /* Process with normal L2/L3 switching. */
    OFPP_FLOOD = 0xfffffffb,  /* All physical ports in VLAN, except input
                   port and those blocked or link down. */
    OFPP_ALL = 0xfffffffc,        /* All physical ports except input port. */
    OFPP_CONTROLLER = 0xfffffffd, /* Send to controller. */
    OFPP_LOCAL = 0xfffffffe,      /* Local openflow "port". */
    OFPP_ANY = 0xffffffff         /* Wildcard port used only for flow mod
             (delete) and flow stats requests. Selects
           all flows regardless of output port
       (including flows with no output port). */
};

/* Features of ports available in a datapath. */
enum ofp_port_features {
    OFPPF_10MB_HD = 1 << 0,  /* 10 Mb half-duplex rate support. */
    OFPPF_10MB_FD = 1 << 1,  /* 10 Mb full-duplex rate support. */
    OFPPF_100MB_HD = 1 << 2, /* 100 Mb half-duplex rate support. */
    OFPPF_100MB_FD = 1 << 3, /* 100 Mb full-duplex rate support. */
    OFPPF_1GB_HD = 1 << 4,   /* 1 Gb half-duplex rate support. */
    OFPPF_1GB_FD = 1 << 5,   /* 1 Gb full-duplex rate support. */
    OFPPF_10GB_FD = 1 << 6,  /* 10 Gb full-duplex rate support. */
    OFPPF_40GB_FD = 1 << 7,  /* 40 Gb full-duplex rate support. */
    OFPPF_100GB_FD = 1 << 8, /* 100 Gb full-duplex rate support. */
    OFPPF_1TB_FD = 1 << 9,   /* 1 Tb full-duplex rate support. */
    OFPPF_OTHER = 1 << 10,   /* Other rate, not in the list. */

    OFPPF_COPPER = 1 << 11,    /* Copper medium. */
    OFPPF_FIBER = 1 << 12,     /* Fiber medium. */
    OFPPF_AUTONEG = 1 << 13,   /* Auto-negotiation. */
    OFPPF_PAUSE = 1 << 14,     /* Pause. */
    OFPPF_PAUSE_ASYM = 1 << 15 /* Asymmetric pause. */
};

enum ofp_controller_max_len {
    OFPCML_MAX = 0xffe5, /* maximum max_len value which can be used
                            to request a specific byte length. */
    OFPCML_NO_BUFFER = 0xffff /* indicates that no buffering should be
                                 applied and the whole packet is to be
                                 sent to the controller. */
};

namespace deprecated { // <namespace deprecated>

// There are differences in message types between versions. Here is a summary
// listing the differences between v4 and each of the earlier versions:
//
//   v3: 0-25 are the same; v3 does not support 26-29.
//       OFPT_MULTIPART_REQUEST replaces OFPT_STATS_REQUEST
//       OFPT_MULTIPART_REPLY replaces OFPT_STATS_REPLY
//   v2: 0-23 are the same; v2 does not support 24-29.
//   v1: 0-14 are the same; 15-21 are different; does not support 22-29.
//       OFPT_EXPERIMENTER replaces OFPT_VENDOR
//       OFPT_PORT_MOD changes to 15; no OFPT_GROUP_MOD or OFPT_TABLE_MOD
//       STATS_REQUEST/REPLY, BARRIER_REQUEST/REPLY, and
//       QUEUE_GET_CONFIG_REQUEST/REPLY are all different.

namespace v1 { // <namespace v1>
enum OFPType : UInt8 {
    OFPT_HELLO = 0,        // Symmetric message
    OFPT_ERROR = 1,        // Symmetric message
    OFPT_ECHO_REQUEST = 2, // Symmetric message
    OFPT_ECHO_REPLY = 3,   // Symmetric message
    OFPT_VENDOR = 4,       // Symmetric message (OFPT_EXPERIMENTER)

    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST = 5,   /* Controller/switch message */
    OFPT_FEATURES_REPLY = 6,     /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST = 7, /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY = 8,   /* Controller/switch message */
    OFPT_SET_CONFIG = 9,         /* Controller/switch message */
    /* Asynchronous messages. */
    OFPT_PACKET_IN = 10,
    OFPT_FLOW_REMOVED = 11,
    OFPT_PORT_STATUS = 12,
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,
    OFPT_FLOW_MOD = 14,
    OFPT_PORT_MOD = 15,
    /* Statistics messages. */
    OFPT_STATS_REQUEST = 16,
    OFPT_STATS_REPLY = 17,
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 18,
    OFPT_BARRIER_REPLY = 19,
    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST = 20, /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY = 21,   /* Controller/switch message */
    OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY
};
} // </namespace v1>

namespace v2 { // <namespace v2>
enum : UInt8 {
    /* Immutable messages. */
    OFPT_HELLO = 0,
    OFPT_ERROR = 1,
    OFPT_ECHO_REQUEST = 2,
    OFPT_ECHO_REPLY = 3,
    OFPT_EXPERIMENTER = 4,
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST = 5,   /* Controller/switch message */
    OFPT_FEATURES_REPLY = 6,     /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST = 7, /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY = 8,   /* Controller/switch message */
    OFPT_SET_CONFIG = 9,         /* Controller/switch message */

    /* Asynchronous messages. */
    OFPT_PACKET_IN = 10,
    OFPT_FLOW_REMOVED = 11,
    OFPT_PORT_STATUS = 12,
    /* Async message */
    /* Async message */
    /* Async message */
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,
    OFPT_FLOW_MOD = 14,
    OFPT_GROUP_MOD = 15,
    OFPT_PORT_MOD = 16,
    OFPT_TABLE_MOD = 17,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Statistics messages. */
    OFPT_STATS_REQUEST = 18,
    OFPT_STATS_REPLY = 19,
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 20,
    OFPT_BARRIER_REPLY = 21,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST = 22, /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY = 23,   /* Controller/switch message */
    OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY
};
} // </namespace v2>

namespace v3 { // <namespace v3>
enum : UInt8 {
    /* Immutable messages. */
    OFPT_HELLO = 0,
    OFPT_ERROR = 1,
    OFPT_ECHO_REQUEST = 2,
    OFPT_ECHO_REPLY = 3,
    OFPT_EXPERIMENTER = 4,
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Symmetric message */
    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST = 5,   /* Controller/switch message */
    OFPT_FEATURES_REPLY = 6,     /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST = 7, /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY = 8,   /* Controller/switch message */
    OFPT_SET_CONFIG = 9,         /* Controller/switch message */

    /* Asynchronous messages. */
    OFPT_PACKET_IN = 10,
    OFPT_FLOW_REMOVED = 11,
    OFPT_PORT_STATUS = 12,
    /* Async message */
    /* Async message */
    /* Async message */
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,
    OFPT_FLOW_MOD = 14,
    OFPT_GROUP_MOD = 15,
    OFPT_PORT_MOD = 16,
    OFPT_TABLE_MOD = 17,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Statistics messages. */
    OFPT_STATS_REQUEST = 18,
    OFPT_STATS_REPLY = 19,
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 20,
    OFPT_BARRIER_REPLY = 21,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST = 22, /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY = 23,   /* Controller/switch message */

    /* Controller role change request messages. */
    OFPT_ROLE_REQUEST = 24, /* Controller/switch message */
    OFPT_ROLE_REPLY = 25,   /* Controller/switch message */
    OFPT_LAST = OFPT_ROLE_REPLY
};
} // </namespace v3>

} // </namespace deprecated>

#if 0
/* Values for 'type' in ofp_error_message.  These values are immutable: they
 * will not change in future versions of the protocol (although new values may
 * be added). */

enum {
    OFPET_HELLO_FAILED = 0, /* Hello protocol failed. */
    OFPET_BAD_REQUEST = 1, /* Request was not understood. */
    OFPET_BAD_ACTION = 2, /* Error in action description. */
    OFPET_BAD_INSTRUCTION = 3, /* Error in instruction list. */
    OFPET_BAD_MATCH = 4, /* Error in match. */
    OFPET_FLOW_MOD_FAILED = 5, /* Problem modifying flow entry. */
    OFPET_GROUP_MOD_FAILED = 6, /* Problem modifying group entry. */
    OFPET_PORT_MOD_FAILED = 7, /* Port mod request failed. */
    OFPET_TABLE_MOD_FAILED = 8, /* Table mod request failed. */
    OFPET_QUEUE_OP_FAILED = 9, /* Queue operation failed. */
    OFPET_SWITCH_CONFIG_FAILED = 10, /* Switch config request failed. */
    OFPET_ROLE_REQUEST_FAILED = 11, /* Controller Role request failed. */
    OFPET_METER_MOD_FAILED = 12, /* Error in meter. */
    OFPET_TABLE_FEATURES_FAILED = 13, /* Setting table features failed. */
    OFPET_EXPERIMENTER = 0xffff /* Experimenter error messages. */
};

/* ofp_error_msg 'code' values for OFPET_HELLO_FAILED.  'data' contains an
 * ASCII text string that may give failure details. */
enum ofp_hello_failed_code {
    OFPHFC_INCOMPATIBLE = 0, /* No compatible version. */
    OFPHFC_EPERM = 1, /* Permissions error. */
};
#endif

/* Hello elements types.
 */
enum ofp_hello_elem_type {
    OFPHET_VERSIONBITMAP = 1, /* Bitmap of version supported. */
};

/* Values for 'type' in ofp_error_message.  These values are immutable: they
 * will not change in future versions of the protocol (although new values may
 * be added). */
enum ofp_error_type {
    OFPET_HELLO_FAILED = 0,           /* Hello protocol failed. */
    OFPET_BAD_REQUEST = 1,            /* Request was not understood. */
    OFPET_BAD_ACTION = 2,             /* Error in action description. */
    OFPET_BAD_INSTRUCTION = 3,        /* Error in instruction list. */
    OFPET_BAD_MATCH = 4,              /* Error in match. */
    OFPET_FLOW_MOD_FAILED = 5,        /* Problem modifying flow entry. */
    OFPET_GROUP_MOD_FAILED = 6,       /* Problem modifying group entry. */
    OFPET_PORT_MOD_FAILED = 7,        /* Port mod request failed. */
    OFPET_TABLE_MOD_FAILED = 8,       /* Table mod request failed. */
    OFPET_QUEUE_OP_FAILED = 9,        /* Queue operation failed. */
    OFPET_SWITCH_CONFIG_FAILED = 10,  /* Switch config request failed. */
    OFPET_ROLE_REQUEST_FAILED = 11,   /* Controller Role request failed. */
    OFPET_METER_MOD_FAILED = 12,      /* Error in meter. */
    OFPET_TABLE_FEATURES_FAILED = 13, /* Setting table features failed. */
    OFPET_EXPERIMENTER = 0xffff       /* Experimenter error messages. */
};

/* ofp_error_msg 'code' values for OFPET_HELLO_FAILED.  'data' contains an
 * ASCII text string that may give failure details. */
enum ofp_hello_failed_code {
    OFPHFC_INCOMPATIBLE = 0, /* No compatible version. */
    OFPHFC_EPERM = 1,        /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_BAD_REQUEST.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum ofp_bad_request_code {
    OFPBRC_BAD_VERSION = 0,      /* ofp_header.version not supported. */
    OFPBRC_BAD_TYPE = 1,         /* ofp_header.type not supported. */
    OFPBRC_BAD_MULTIPART = 2,    /* ofp_multipart_request.type not supported */
    OFPBRC_BAD_EXPERIMENTER = 3, /* Experimenter id not supported
* (in ofp_experimenter_header or
* ofp_multipart_request or
* ofp_multipart_reply). */

    OFPBRC_BAD_EXP_TYPE = 4,   /* Experimenter type not supported. */
    OFPBRC_EPERM = 5,          /* Permissions error. */
    OFPBRC_BAD_LEN = 6,        /* Wrong request length for type. */
    OFPBRC_BUFFER_EMPTY = 7,   /* Specified buffer has already been used. */
    OFPBRC_BUFFER_UNKNOWN = 8, /* Specified buffer does not exist. */
    OFPBRC_BAD_TABLE_ID = 9,   /* Specified
table-id invalid or does not exist. */
    OFPBRC_IS_SLAVE = 10,   /* Denied because controller is slave. */
    OFPBRC_BAD_PORT = 11,   /* Invalid port. */
    OFPBRC_BAD_PACKET = 12, /* Invalid packet in packet-out. */
    OFPBRC_MULTIPART_BUFFER_OVERFLOW = 13, /* ofp_multipart_request
    overflowed the assigned buffer. */
};

/* ofp_error_msg 'code' values for OFPET_BAD_ACTION.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum ofp_bad_action_code {
    OFPBAC_BAD_TYPE = 0,            /* Unknown action type. */
    OFPBAC_BAD_LEN = 1,             /* Length problem in actions. */
    OFPBAC_BAD_EXPERIMENTER = 2,    /* Unknown experimenter id specified. */
    OFPBAC_BAD_EXP_TYPE = 3,        /* Unknown action for experimenter id. */
    OFPBAC_BAD_OUT_PORT = 4,        /* Problem validating output port. */
    OFPBAC_BAD_ARGUMENT = 5,        /* Bad action argument. */
    OFPBAC_EPERM = 6,               /* Permissions error. */
    OFPBAC_TOO_MANY = 7,            /* Can't handle this many actions. */
    OFPBAC_BAD_QUEUE = 8,           /* Problem validating output queue. */
    OFPBAC_BAD_OUT_GROUP = 9,       /* Invalid group id in forward action. */
    OFPBAC_MATCH_INCONSISTENT = 10, /* Action can't apply for this match,
                                       or Set-Field missing prerequisite. */
    OFPBAC_UNSUPPORTED_ORDER = 11, /* Action order is unsupported for the
   action list in an Apply-Actions instruction */
    OFPBAC_BAD_TAG = 12, /* Actions uses an unsupported
tag/encap. */
    OFPBAC_BAD_SET_TYPE = 13,     /* Unsupported type in SET_FIELD action. */
    OFPBAC_BAD_SET_LEN = 14,      /* Length problem in SET_FIELD action. */
    OFPBAC_BAD_SET_ARGUMENT = 15, /* Bad argument in SET_FIELD action. */
};

/* ofp_error_msg 'code' values for OFPET_BAD_INSTRUCTION.  'data' contains at
 * least
 * the first 64 bytes of the failed request. */
enum ofp_bad_instruction_code {
    OFPBIC_UNKNOWN_INST = 0, /* Unknown instruction. */
    OFPBIC_UNSUP_INST = 1,   /* Switch or table does not support the
instruction. */
    OFPBIC_BAD_TABLE_ID = 2,   /* Invalid Table-ID specified. */
    OFPBIC_UNSUP_METADATA = 3, /* Metadata value unsupported by datapath. */
    OFPBIC_UNSUP_METADATA_MASK = 4, /* Metadata mask value unsupported by
                                       datapath. */
    OFPBIC_BAD_EXPERIMENTER = 5, /* Unknown experimenter id specified. */
    OFPBIC_BAD_EXP_TYPE = 6,     /* Unknown instruction for experimenter id. */
    OFPBIC_BAD_LEN = 7,          /* Length problem in instructions. */
    OFPBIC_EPERM = 8,            /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_BAD_MATCH.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum ofp_bad_match_code {
    OFPBMC_BAD_TYPE = 0, /* Unsupported match type specified by the
match */

    OFPBMC_BAD_LEN = 1,          /* Length problem in match. */
    OFPBMC_BAD_TAG = 2,          /* Match uses an unsupported tag/encap. */
    OFPBMC_BAD_DL_ADDR_MASK = 3, /* Unsupported datalink addr mask - switch
                                     does not support arbitrary datalink
                                     address mask. */
    OFPBMC_BAD_NW_ADDR_MASK =
        4, /* Unsupported network addr mask - switch does not
support arbitrary network
address mask. */
    OFPBMC_BAD_WILDCARDS = 5, /* Unsupported combination of fields masked
       or omitted in the match. */
    OFPBMC_BAD_FIELD = 6, /* Unsupported field type in the match. */
    OFPBMC_BAD_VALUE = 7, /* Unsupported value in a match field. */
    OFPBMC_BAD_MASK = 8,  /* Unsupported mask specified in the match,
field is not dl-address or nw-address. */
    OFPBMC_BAD_PREREQ = 9, /* A prerequisite was not met. */
    OFPBMC_DUP_FIELD = 10, /* A field type was duplicated. */
    OFPBMC_EPERM = 11,     /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_FLOW_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */

enum ofp_flow_mod_failed_code {
    OFPFMFC_UNKNOWN = 0,      /* Unspecified error. */
    OFPFMFC_TABLE_FULL = 1,   /* Flow not added because table was full. */
    OFPFMFC_BAD_TABLE_ID = 2, /* Table does not exist */
    OFPFMFC_OVERLAP = 3,      /* Attempted to add overlapping flow with
CHECK_OVERLAP flag set. */
    OFPFMFC_EPERM = 4,       /* Permissions error. */
    OFPFMFC_BAD_TIMEOUT = 5, /* Flow not added because of unsupported
         idle/hard timeout. */
    OFPFMFC_BAD_COMMAND = 6, /* Unsupported or unknown command. */
    OFPFMFC_BAD_FLAGS = 7,   /* Unsupported or unknown flags. */
};

/* ofp_error_msg 'code' values for OFPET_GROUP_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum ofp_group_mod_failed_code {
    OFPGMFC_GROUP_EXISTS =
        0, /* Group not added because a group ADD attempted to replace an
already-present group. */
    OFPGMFC_INVALID_GROUP = 1, /* Group not added because Group
specified is invalid. */
    OFPGMFC_WEIGHT_UNSUPPORTED = 2, /* Switch does not support unequal load
   sharing with select groups. */
    OFPGMFC_OUT_OF_GROUPS = 3,  /* The group table is full. */
    OFPGMFC_OUT_OF_BUCKETS = 4, /* The maximum number of action buckets
                         for a group has been exceeded. */
    OFPGMFC_CHAINING_UNSUPPORTED = 5, /* Switch does not support groups that
                                 forward to groups. */

    OFPGMFC_WATCH_UNSUPPORTED =
        6, /* This group cannot watch the watch_port or watch_group specified.
              */
    OFPGMFC_LOOP = 7,          /* Group entry would cause a loop. */
    OFPGMFC_UNKNOWN_GROUP = 8, /* Group not modified because a group
                MODIFY attempted to modify a
       non-existent group. */
    OFPGMFC_CHAINED_GROUP = 9, /* Group not deleted because another
       group is forwarding to it. */
    OFPGMFC_BAD_TYPE = 10,    /* Unsupported or unknown group type. */
    OFPGMFC_BAD_COMMAND = 11, /* Unsupported or unknown command. */
    OFPGMFC_BAD_BUCKET = 12,  /* Error in bucket. */
    OFPGMFC_BAD_WATCH = 13,   /* Error in watch port/group. */
    OFPGMFC_EPERM = 14,       /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_PORT_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum ofp_port_mod_failed_code {
    OFPPMFC_BAD_PORT = 0,    /* Specified port number does not exist. */
    OFPPMFC_BAD_HW_ADDR = 1, /* Specified hardware address does not
   * match the port number. */
    OFPPMFC_BAD_CONFIG = 2,    /* Specified config is invalid. */
    OFPPMFC_BAD_ADVERTISE = 3, /* Specified advertise is invalid. */
    OFPPMFC_EPERM = 4,         /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_TABLE_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum ofp_table_mod_failed_code {
    OFPTMFC_BAD_TABLE = 0,  /* Specified table does not exist. */
    OFPTMFC_BAD_CONFIG = 1, /* Specified config is invalid. */
    OFPTMFC_EPERM = 2,      /* Permissions error. */
};

/* ofp_error msg 'code' values for OFPET_QUEUE_OP_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request */

enum ofp_queue_op_failed_code {
    OFPQOFC_BAD_PORT = 0,  /* Invalid port (or port does not exist). */
    OFPQOFC_BAD_QUEUE = 1, /* Queue does not exist. */
    OFPQOFC_EPERM = 2,     /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_SWITCH_CONFIG_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request. */

enum ofp_switch_config_failed_code {
    OFPSCFC_BAD_FLAGS = 0, /* Specified flags is invalid. */
    OFPSCFC_BAD_LEN = 1,   /* Specified len is invalid. */
    OFPSCFC_EPERM = 2,     /* Permissions error. */
};

/* ofp_error_msg 'code' values for OFPET_ROLE_REQUEST_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request. */

enum ofp_role_request_failed_code {
    OFPRRFC_STALE = 0,    /* Stale Message: old generation_id. */
    OFPRRFC_UNSUP = 1,    /* Controller role change unsupported. */
    OFPRRFC_BAD_ROLE = 2, /* Invalid role. */
};

/* ofp_error_msg 'code' values for OFPET_METER_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */

enum ofp_meter_mod_failed_code {
    OFPMMFC_UNKNOWN = 0,      /* Unspecified error. */
    OFPMMFC_METER_EXISTS = 1, /* Meter not added because a Meter ADD
                                 attempted to replace an existing Meter. */
    OFPMMFC_INVALID_METER = 2, /* Meter not added because Meter specified
                                  is invalid. */
    OFPMMFC_UNKNOWN_METER = 3, /* Meter not modified because a Meter
                                  MODIFY attempted to modify a non-existent
                                  Meter. */

    OFPMMFC_BAD_COMMAND = 4,    /* Unsupported or unknown command. */
    OFPMMFC_BAD_FLAGS = 5,      /* Flag configuration unsupported. */
    OFPMMFC_BAD_RATE = 6,       /* Rate unsupported. */
    OFPMMFC_BAD_BURST = 7,      /* Burst size unsupported. */
    OFPMMFC_BAD_BAND = 8,       /* Band unsupported. */
    OFPMMFC_BAD_BAND_VALUE = 9, /* Band value unsupported. */
    OFPMMFC_OUT_OF_METERS = 10, /* No more meters available. */
    OFPMMFC_OUT_OF_BANDS = 11,  /* The maximum number of properties
                           for a meter has been exceeded. */
};

/* ofp_error_msg 'code' values for OFPET_TABLE_FEATURES_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request. */

enum ofp_table_features_failed_code {
    OFPTFFC_BAD_TABLE = 0,    /* Specified table does not exist. */
    OFPTFFC_BAD_METADATA = 1, /* Invalid metadata mask. */
    OFPTFFC_BAD_TYPE = 2,     /* Unknown property type. */
    OFPTFFC_BAD_LEN = 3,      /* Length problem in properties. */
    OFPTFFC_BAD_ARGUMENT = 4, /* Unsupported property value. */
    OFPTFFC_EPERM = 5,        /* Permissions error. */
};

/* Table numbering. Tables can use any number up to OFPT_MAX. */
enum ofp_table {
    /* Last usable table number. */
    OFPTT_MAX = 0xfe,
    /* Fake tables. */
    OFPTT_ALL = 0xff /* Wildcard table used for table config,
                        flow stats and flow deletes. */
};

/* Group numbering. Groups can use any number up to OFPG_MAX. */
enum ofp_group {
    /* Last usable group number. */
    OFPG_MAX = 0xffffff00,
    /* Fake groups. */
    OFPG_ALL = 0xfffffffc, /* Represents all groups for group delete
                      commands. */
    OFPG_ANY = 0xffffffff
    /* Wildcard group used only for flow stats
       requests. Selects all flows regardless of
       group (including flows with no group).
       */
};

enum OFPMultipartType : UInt16 {
    /* Description of this OpenFlow switch.
     * The request body is empty.
     * The reply body is struct ofp_desc. */
    OFPMP_DESC = 0,
    /* Individual flow statistics.
     * The request body is struct ofp_flow_stats_request.
     * The reply body is an array of struct ofp_flow_stats. */
    OFPMP_FLOW = 1,
    /* Aggregate flow statistics.
     * The request body is struct ofp_aggregate_stats_request.
     * The reply body is struct ofp_aggregate_stats_reply. */
    OFPMP_AGGREGATE = 2,
    /* Flow table statistics.
     * The request body is empty.
     * The reply body is an array of struct ofp_table_stats. */
    OFPMP_TABLE = 3,
    /* Port statistics.
     * The request body is struct ofp_port_stats_request.
     * The reply body is an array of struct ofp_port_stats. */
    OFPMP_PORT_STATS = 4,
    /* Queue statistics for a port
     * The request body is struct ofp_queue_stats_request.
     * The reply body is an array of struct ofp_queue_stats */
    OFPMP_QUEUE = 5,
    /* Group counter statistics.
     * The request body is struct ofp_group_stats_request.
     * The reply is an array of struct ofp_group_stats. */
    OFPMP_GROUP = 6,
    /* Group description.
     * The request body is empty.
     * The reply body is an array of struct ofp_group_desc. */
    OFPMP_GROUP_DESC = 7,
    /* Group features.
     * The request body is empty.
     * The reply body is struct ofp_group_features. */
    OFPMP_GROUP_FEATURES = 8,
    /* Meter statistics.
     * The request body is struct ofp_meter_multipart_requests.
     * The reply body is an array of struct ofp_meter_stats. */
    OFPMP_METER = 9,
    /* Meter configuration.
     * The request body is struct ofp_meter_multipart_requests.
     * The reply body is an array of struct ofp_meter_config. */
    OFPMP_METER_CONFIG = 10,
    /* Meter features.
     * The request body is empty.
     * The reply body is struct ofp_meter_features. */
    OFPMP_METER_FEATURES = 11,
    /* Table features.
     * The request body is either empty or contains an array of
     * struct ofp_table_features containing the controller's
     * desired view of the switch. If the switch is unable to
     * set the specified view an error is returned.
     * The reply body is an array of struct ofp_table_features. */
    OFPMP_TABLE_FEATURES = 12,
    /* Port description.
     * The request body is empty.
     * The reply body is an array of struct ofp_port. */
    OFPMP_PORT_DESC = 13,
    /* Experimenter extension.
     * The request and reply bodies begin with
     * struct ofp_experimenter_multipart_header.
     * The request and reply bodies are otherwise experimenter-defined. */
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
    OFPAT_SET_FIELD = 25, // 32 possible lengths: 8, 16, 24, 32, 40, ..., 256
    OFPAT_PUSH_PBB = 26,   // New in v4
    OFPAT_POP_PBB = 27,     // New in v4

    // "Fake" Version 1 actions.
    OFPAT_STRIP_VLAN_V1 = 0x7FF1,
    OFPAT_ENQUEUE_V1 = 0x7FF2,

    OFPAT_EXPERIMENTER = 0xFFFF // possible lengths: 8, 16, 24, 32, 40, ...
};


namespace deprecated { // <namespace deprecated>

// Version               1       2      3      4
// ----------------------------------------------
// OFPAT_OUTPUT          0.8     0.16   0.16   0.16   (*)
// OFPAT_SET_VLAN_VID    1.8     1.8    -      -
// OFPAT_SET_VLAN_PCP    2.8     2.8    -      -
// OFPAT_STRIP_VLAN      3.8     -      -      -     <---- Not Implemented
// OFPAT_SET_DL_SRC      4.16    3.16   -      -
// OFPAT_SET_DL_DST      5.16    4.16   -      -
// OFPAT_SET_NW_SRC      6       5      -      -
// OFPAT_SET_NW_DST      7       6      -      -
// OFPAT_SET_NW_TOS      8       7      -      -
// OFPAT_SET_NW_ECN      -       8      -      -
// OFPAT_SET_TP_SRC      9       9      -      -
// OFPAT_SET_TP_DST     10      10      -      -
// OFPAT_ENQUEUE        11       -      -      -     <---- Not Implemented
// OFPAT_COPY_TTL_OUT    -      11     11     11
// OFPAT_COPY_TTL_IN     -      12     12     12
// OFPAT_SET_MPLS_LABEL  -      13      -      -
// OFPAT_SET_MPLS_TC     -      14      -      -
// OFPAT_SET_MPLS_TTL    -      15     15     15
// OFPAT_DEC_MPLS_TTL    -      16     16     16
// OFPAT_PUSH_VLAN       -      17     17     17
// OFPAT_POP_VLAN        -      18     18     18
// OFPAT_PUSH_MPLS       -      19     19     19
// OFPAT_POP_MPLS        -      20     20     20
// OFPAT_SET_QUEUE       -      21     21     21
// OFPAT_GROUP           -      22     22     22
// OFPAT_SET_NW_TTL      -      23     23     23
// OFPAT_DEC_NW_TTL      -      24     24     24
// OFPAT_SET_FIELD       -       -     25     25
// OFPAT_PUSH_PBB        -       -      -     26
// OFPAT_POP_PBB         -       -      -     27
// OFPAT_EXPERIMENTER    -     0xFFFF 0xFFFF 0xFFFF
//
// (*) Port field changed from 16 to 32 bits in v2.

namespace v1 { // <namespace v1>
enum : UInt16 {
    OFPAT_OUTPUT = 0,
    OFPAT_SET_VLAN_VID = 1,
    OFPAT_SET_VLAN_PCP = 2,
    OFPAT_STRIP_VLAN = 3,   // only present in v1
    OFPAT_SET_DL_SRC = 4,   // these constants differ from later versions
    OFPAT_SET_DL_DST = 5,
    OFPAT_SET_NW_SRC = 6,
    OFPAT_SET_NW_DST = 7,
    OFPAT_SET_NW_TOS = 8,
    OFPAT_SET_TP_SRC = 9,
    OFPAT_SET_TP_DST = 10,
    OFPAT_ENQUEUE = 11      // only present in v1
};
} // </namespace v1>

namespace v2 { // <namespace v2>
enum : UInt16 {
    OFPAT_OUTPUT = 0,
    OFPAT_SET_VLAN_VID = 1,
    OFPAT_SET_VLAN_PCP = 2,
    OFPAT_SET_DL_SRC = 3,    // different from v1
    OFPAT_SET_DL_DST = 4,
    OFPAT_SET_NW_SRC = 5,
    OFPAT_SET_NW_DST = 6,
    OFPAT_SET_NW_TOS = 7,
    OFPAT_SET_NW_ECN = 8,    // new in v2
    OFPAT_SET_TP_SRC = 9,
    OFPAT_SET_TP_DST = 10,
    OFPAT_COPY_TTL_OUT = 11,  // changed in v2
    OFPAT_COPY_TTL_IN = 12,     // new in v2 from here on...
    OFPAT_SET_MPLS_LABEL = 13,  
    OFPAT_SET_MPLS_TC = 14,
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

    OFPAT_EXPERIMENTER = 0xFFFF
};
} // </namespace v2>

namespace v3 { // <namespace v3>
enum : UInt16 {
    OFPAT_OUTPUT = 0,
    // -OFPAT_SET_VLAN_VID = 1,
    // -OFPAT_SET_VLAN_PCP = 2,
    // -OFPAT_SET_DL_SRC = 3,
    // -OFPAT_SET_DL_DST = 4,
    // -OFPAT_SET_NW_SRC = 5,
    // -OFPAT_SET_NW_DST = 6,
    // -OFPAT_SET_NW_TOS = 7,
    // -OFPAT_SET_NW_ECN = 8,
    // -OFPAT_SET_TP_SRC = 9,
    // -OFPAT_SET_TP_DST = 10,
    OFPAT_COPY_TTL_OUT = 11,
    OFPAT_COPY_TTL_IN = 12,
    // -OFPAT_SET_MPLS_LABEL = 13,  
    // -OFPAT_SET_MPLS_TC = 14,
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
    OFPAT_SET_FIELD = 25,

    OFPAT_EXPERIMENTER = 0xFFFF
};
} // </namespace v3>

} // </namespace deprecated>

enum OFPPacketInReason : UInt8 {
    OFPR_NO_MATCH    = 0,       //< No matching flow (table-miss flow entry)
    OFPR_ACTION      = 1,       //< Action explicitly output to controller
    OFPR_INVALID_TTL = 2,       //< Packet has invalid TTL (added in v1.2)
};


enum OFPFlowModCommand : UInt8 {
    OFPFC_ADD = 0,
    OFPFC_MODIFY = 1,
    OFPFC_MODIFY_STRICT = 2,
    OFPFC_DELETE = 3,
    OFPFC_DELETE_STRICT = 4
};

} // </namespace ofp>

#endif // OFP_CONSTANTS_H
