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
#include "ofp/error.h"

using namespace ofp;

OFPMultipartType Message::subtype() const {
  OFPType mtype = type();
  if ((mtype == OFPT_MULTIPART_REQUEST || mtype == OFPT_MULTIPART_REPLY) &&
      buf_.size() >= 12) {
    UInt16 subtype = *Big16_cast(buf_.data() + 8);
    return static_cast<OFPMultipartType>(subtype);
  } else {
    return OFPMP_UNSUPPORTED;
  }
}

OFPMultipartFlags Message::flags() const {
  OFPType mtype = type();
  if ((mtype == OFPT_MULTIPART_REQUEST || mtype == OFPT_MULTIPART_REPLY) &&
      buf_.size() >= 12) {
    UInt16 flags = *Big16_cast(buf_.data() + 10);
    return static_cast<OFPMultipartFlags>(flags);
  } else {
    return OFPMPF_NONE;
  }
}

Channel *Message::source() const {
  return channel_;
}

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

void Message::replyError(OFPErrorCode error,
                         const std::string &explanation) const {
  // Never reply to an Error message with an Error.
  if (source() && type() != OFPT_ERROR) {
    ErrorBuilder errorBuilder{xid()};
    errorBuilder.setErrorCode(error);
    if (!explanation.empty()) {
      errorBuilder.setErrorData(explanation.data(), explanation.size());
    } else {
      errorBuilder.setErrorData(this);
    }
    errorBuilder.send(source());
  }
}
