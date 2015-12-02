// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPFLOWMONITORREPLY_H_
#define OFP_YAML_YMPFLOWMONITORREPLY_H_

#include "ofp/mpflowmonitorreply.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowMonitorReply> {
  static void mapping(IO &io, ofp::MPFlowMonitorReply &msg) {
    ofp::OFPFlowUpdateEvent event = msg.event();
    io.mapRequired("event", event);

    if (event <= ofp::OFPFME_MODIFIED) {
      io.mapRequired("table_id", msg.tableId_);
      io.mapRequired("reason", msg.reason_);
      io.mapRequired("idle_timeout", msg.idleTimeout_);
      io.mapRequired("hard_timeout", msg.hardTimeout_);
      io.mapRequired("priority", msg.priority_);
      io.mapRequired("cookie", msg.cookie_);

      ofp::Match m = msg.match();
      io.mapRequired("match", m);

      ofp::InstructionRange instructions = msg.instructions();
      io.mapRequired("instructions", instructions);

    } else if (event == ofp::OFPFME_ABBREV) {
      Hex32 xid = msg.xid();
      io.mapRequired("xid", xid);
    }
  }
};

template <>
struct MappingTraits<ofp::MPFlowMonitorReplyBuilder> {
  static void mapping(IO &io, ofp::MPFlowMonitorReplyBuilder &msg) {
    ofp::OFPFlowUpdateEvent event = ofp::OFPFME_INITIAL;
    io.mapRequired("event", event);
    msg.setEvent(event);

    if (event <= ofp::OFPFME_MODIFIED) {
      io.mapRequired("table_id", msg.msg_.tableId_);
      io.mapRequired("reason", msg.msg_.reason_);
      io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
      io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
      io.mapRequired("priority", msg.msg_.priority_);
      io.mapRequired("cookie", msg.msg_.cookie_);

      io.mapRequired("match", msg.match_);
      io.mapRequired("instructions", msg.instructions_);

    } else if (event == ofp::OFPFME_ABBREV) {
      Hex32 xid;
      io.mapRequired("xid", xid);
      msg.setXid(xid);
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPFLOWMONITORREPLY_H_
