// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/flowmod.h"
#include "ofp/portstatus.h"
#include "ofp/experimenter.h"
#include "ofp/instructions.h"
#include "ofp/instructionrange.h"
#include "ofp/originalmatch.h"
#include "ofp/transmogrify.h"

using namespace ofp;

Channel *Message::source() const { return channel_; }

bool Message::isRequestType() const {
  // Echo request is not included because it's handled by the connection
  // itself.

  switch (type()) {
    case OFPT_FEATURES_REQUEST:
    case OFPT_GET_CONFIG_REQUEST:
    case OFPT_MULTIPART_REQUEST:
    case OFPT_BARRIER_REQUEST:
    case OFPT_QUEUE_GET_CONFIG_REQUEST:
    case OFPT_ROLE_REQUEST:
    case OFPT_GET_ASYNC_REQUEST:
      return true;
    default:
      break;
  }

  return false;
}

void Message::transmogrify() {
  Transmogrify tr{this};
  tr.normalize();
}
