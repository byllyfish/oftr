// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_CONSTANTS_DEPRECATED_H_
#define OFP_CONSTANTS_DEPRECATED_H_

namespace ofp {
namespace deprecated {

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

namespace v1 {

enum OFPType : UInt8 {
  OFPT_HELLO = 0,         // Symmetric message
  OFPT_ERROR = 1,         // Symmetric message
  OFPT_ECHO_REQUEST = 2,  // Symmetric message
  OFPT_ECHO_REPLY = 3,    // Symmetric message
  OFPT_VENDOR = 4,        // Symmetric message (OFPT_EXPERIMENTER)

  // Switch configuration messages.
  OFPT_FEATURES_REQUEST = 5,    // Controller/switch message
  OFPT_FEATURES_REPLY = 6,      // Controller/switch message
  OFPT_GET_CONFIG_REQUEST = 7,  // Controller/switch message
  OFPT_GET_CONFIG_REPLY = 8,    // Controller/switch message
  OFPT_SET_CONFIG = 9,          // Controller/switch message
  // Asynchronous messages.
  OFPT_PACKET_IN = 10,
  OFPT_FLOW_REMOVED = 11,
  OFPT_PORT_STATUS = 12,
  // Controller command messages.
  OFPT_PACKET_OUT = 13,
  OFPT_FLOW_MOD = 14,
  OFPT_PORT_MOD = 15,
  // Statistics messages.
  OFPT_STATS_REQUEST = 16,
  OFPT_STATS_REPLY = 17,
  // Barrier messages.
  OFPT_BARRIER_REQUEST = 18,
  OFPT_BARRIER_REPLY = 19,
  // Queue Configuration messages.
  OFPT_QUEUE_GET_CONFIG_REQUEST = 20,  // Controller/switch message
  OFPT_QUEUE_GET_CONFIG_REPLY = 21,    // Controller/switch message
  OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY
};

enum : UInt16 {
  OFPVID_NONE = 0xffff,

  // Not in 1.0 spec, but allow it anyway.
  OFPVID_PRESENT = 0xfffe
};

enum OFPCapabilitiesFlags : UInt32 { OFPC_STP = 1 << 3 };

enum OFPPortFeaturesFlags : UInt32 {
  OFPPF_COPPER = 1 << 7,
  OFPPF_FIBER = 1 << 8,
  OFPPF_AUTONEG = 1 << 9,
  OFPPF_PAUSE = 1 << 10,
  OFPPF_PAUSE_ASYM = 1 << 11
};

}  // namespace v1

namespace v2 {

enum : UInt8 {
  // Immutable messages.
  OFPT_HELLO = 0,
  OFPT_ERROR = 1,
  OFPT_ECHO_REQUEST = 2,
  OFPT_ECHO_REPLY = 3,
  OFPT_EXPERIMENTER = 4,
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Switch configuration messages.
  OFPT_FEATURES_REQUEST = 5,    // Controller/switch message
  OFPT_FEATURES_REPLY = 6,      // Controller/switch message
  OFPT_GET_CONFIG_REQUEST = 7,  // Controller/switch message
  OFPT_GET_CONFIG_REPLY = 8,    // Controller/switch message
  OFPT_SET_CONFIG = 9,          // Controller/switch message

  // Asynchronous messages.
  OFPT_PACKET_IN = 10,
  OFPT_FLOW_REMOVED = 11,
  OFPT_PORT_STATUS = 12,
  // Async message
  // Async message
  // Async message
  // Controller command messages.
  OFPT_PACKET_OUT = 13,
  OFPT_FLOW_MOD = 14,
  OFPT_GROUP_MOD = 15,
  OFPT_PORT_MOD = 16,
  OFPT_TABLE_MOD = 17,
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Statistics messages.
  OFPT_STATS_REQUEST = 18,
  OFPT_STATS_REPLY = 19,
  // Barrier messages.
  OFPT_BARRIER_REQUEST = 20,
  OFPT_BARRIER_REPLY = 21,
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Queue Configuration messages.
  OFPT_QUEUE_GET_CONFIG_REQUEST = 22,  // Controller/switch message
  OFPT_QUEUE_GET_CONFIG_REPLY = 23,    // Controller/switch message
  OFPT_LAST = OFPT_QUEUE_GET_CONFIG_REPLY
};

}  // namespace v2

namespace v3 {

enum : UInt8 {
  // Immutable messages.
  OFPT_HELLO = 0,
  OFPT_ERROR = 1,
  OFPT_ECHO_REQUEST = 2,
  OFPT_ECHO_REPLY = 3,
  OFPT_EXPERIMENTER = 4,
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Symmetric message
  // Switch configuration messages.
  OFPT_FEATURES_REQUEST = 5,    // Controller/switch message
  OFPT_FEATURES_REPLY = 6,      // Controller/switch message
  OFPT_GET_CONFIG_REQUEST = 7,  // Controller/switch message
  OFPT_GET_CONFIG_REPLY = 8,    // Controller/switch message
  OFPT_SET_CONFIG = 9,          // Controller/switch message

  // Asynchronous messages.
  OFPT_PACKET_IN = 10,
  OFPT_FLOW_REMOVED = 11,
  OFPT_PORT_STATUS = 12,
  // Async message
  // Async message
  // Async message
  // Controller command messages.
  OFPT_PACKET_OUT = 13,
  OFPT_FLOW_MOD = 14,
  OFPT_GROUP_MOD = 15,
  OFPT_PORT_MOD = 16,
  OFPT_TABLE_MOD = 17,
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Statistics messages.
  OFPT_STATS_REQUEST = 18,
  OFPT_STATS_REPLY = 19,
  // Barrier messages.
  OFPT_BARRIER_REQUEST = 20,
  OFPT_BARRIER_REPLY = 21,
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Controller/switch message
  // Queue Configuration messages.
  OFPT_QUEUE_GET_CONFIG_REQUEST = 22,  // Controller/switch message
  OFPT_QUEUE_GET_CONFIG_REPLY = 23,    // Controller/switch message

  // Controller role change request messages.
  OFPT_ROLE_REQUEST = 24,  // Controller/switch message
  OFPT_ROLE_REPLY = 25,    // Controller/switch message
  OFPT_LAST = OFPT_ROLE_REPLY
};

}  // namespace v3

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

namespace v1 {

enum : UInt16 {
  OFPAT_OUTPUT = 0,
  OFPAT_SET_VLAN_VID = 1,
  OFPAT_SET_VLAN_PCP = 2,
  OFPAT_STRIP_VLAN = 3,  // only present in v1
  OFPAT_SET_DL_SRC = 4,  // these constants differ from later versions
  OFPAT_SET_DL_DST = 5,
  OFPAT_SET_NW_SRC = 6,
  OFPAT_SET_NW_DST = 7,
  OFPAT_SET_NW_TOS = 8,
  OFPAT_SET_TP_SRC = 9,
  OFPAT_SET_TP_DST = 10,
  OFPAT_ENQUEUE = 11  // only present in v1
};

}  // namespace v1

namespace v2 {

enum : UInt16 {
  OFPAT_OUTPUT = 0,
  OFPAT_SET_VLAN_VID = 1,
  OFPAT_SET_VLAN_PCP = 2,
  OFPAT_SET_DL_SRC = 3,  // different from v1
  OFPAT_SET_DL_DST = 4,
  OFPAT_SET_NW_SRC = 5,
  OFPAT_SET_NW_DST = 6,
  OFPAT_SET_NW_TOS = 7,
  OFPAT_SET_NW_ECN = 8,  // new in v2
  OFPAT_SET_TP_SRC = 9,
  OFPAT_SET_TP_DST = 10,
  OFPAT_COPY_TTL_OUT = 11,  // changed in v2
  OFPAT_COPY_TTL_IN = 12,   // new in v2 from here on...
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

}  // namespace v2

namespace v3 {

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

}  // namespace v3

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_CONSTANTS_DEPRECATED_H_
