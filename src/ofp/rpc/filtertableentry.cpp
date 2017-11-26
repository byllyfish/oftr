// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/filtertableentry.h"
#include "ofp/message.h"
#include "ofp/rpc/filteraction.h"

using namespace ofp::rpc;

bool FilterTableEntry::apply(ByteRange data, PortNumber inPort,
                             Message *message, bool *escalate) {
  assert(message->type() == OFPT_PACKET_IN);

  if (!pktFilter_.match(data)) {
    log_debug("FilterTableEntry::apply - data doesn't match filter");
    return false;
  }

  if (action_) {
    if (!action_->apply(data, inPort, message)) {
      log_debug("FilterTableEntry::apply - action doesn't match");
      return false;
    }
  }

  *escalate = escalate_;
  
  return true;
}
