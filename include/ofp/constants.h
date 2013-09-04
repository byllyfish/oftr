//  ===== ---- ofp/constants.h -----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines all OpenFlow constants.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_CONSTANTS_H
#define OFP_CONSTANTS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

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
//
// FIXME: strong typing?
enum OFPType : UInt8{
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
    OFPT_UNSUPPORTED = 0xF9};

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
//
enum class v1
    : UInt8{OFPT_HELLO = 0,        // Symmetric message
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
            OFPT_PACKET_IN = 10,                    OFPT_FLOW_REMOVED = 11,
            OFPT_PORT_STATUS = 12,
            /* Controller command messages. */
            OFPT_PACKET_OUT = 13,                   OFPT_FLOW_MOD = 14,
            OFPT_PORT_MOD = 15,
            /* Statistics messages. */
            OFPT_STATS_REQUEST = 16,                OFPT_STATS_REPLY = 17,
            /* Barrier messages. */
            OFPT_BARRIER_REQUEST = 18,              OFPT_BARRIER_REPLY = 19,
            /* Queue Configuration messages. */
            OFPT_QUEUE_GET_CONFIG_REQUEST = 20, /* Controller/switch message */
            OFPT_QUEUE_GET_CONFIG_REPLY = 21,   /* Controller/switch message */
            OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY};

enum class v2 : UInt8{
    /* Immutable messages. */
    OFPT_HELLO = 0,                         OFPT_ERROR = 1,
    OFPT_ECHO_REQUEST = 2,                  OFPT_ECHO_REPLY = 3,
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
    OFPT_PACKET_IN = 10,                    OFPT_FLOW_REMOVED = 11,
    OFPT_PORT_STATUS = 12,
    /* Async message */
    /* Async message */
    /* Async message */
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,                   OFPT_FLOW_MOD = 14,
    OFPT_GROUP_MOD = 15,                    OFPT_PORT_MOD = 16,
    OFPT_TABLE_MOD = 17,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Statistics messages. */
    OFPT_STATS_REQUEST = 18,                OFPT_STATS_REPLY = 19,
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 20,              OFPT_BARRIER_REPLY = 21,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST = 22, /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY = 23,   /* Controller/switch message */
    OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY};

enum class v3 : UInt8{
    /* Immutable messages. */
    OFPT_HELLO = 0,                     OFPT_ERROR = 1,
    OFPT_ECHO_REQUEST = 2,              OFPT_ECHO_REPLY = 3,
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
    OFPT_PACKET_IN = 10,                OFPT_FLOW_REMOVED = 11,
    OFPT_PORT_STATUS = 12,
    /* Async message */
    /* Async message */
    /* Async message */
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,               OFPT_FLOW_MOD = 14,
    OFPT_GROUP_MOD = 15,                OFPT_PORT_MOD = 16,
    OFPT_TABLE_MOD = 17,
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Controller/switch message */
    /* Statistics messages. */
    OFPT_STATS_REQUEST = 18,            OFPT_STATS_REPLY = 19,
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 20,          OFPT_BARRIER_REPLY = 21,
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
    OFPT_LAST = OFPT_ROLE_REPLY};

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

} // </namespace ofp>

#endif // OFP_CONSTANTS_H
