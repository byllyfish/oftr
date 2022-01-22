// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/message.h"

#include "ofp/channel.h"
#include "ofp/error.h"
#include "ofp/experimenter.h"
#include "ofp/flowmod.h"
#include "ofp/instructionrange.h"
#include "ofp/instructions.h"
#include "ofp/normalize.h"
#include "ofp/originalmatch.h"
#include "ofp/portstatus.h"

using namespace ofp;

OFPMultipartType Message::subtype() const {
  OFPType mtype = type();
  if ((mtype == OFPT_MULTIPART_REQUEST || mtype == OFPT_MULTIPART_REPLY) &&
      buf_.size() >= 12) {
    UInt16 subtype = *Big16_cast(buf_.data() + 8);
    return static_cast<OFPMultipartType>(subtype);
  }
  return OFPMP_UNSUPPORTED;
}

OFPMultipartFlags Message::multipartFlags() const {
  OFPType mtype = type();
  if ((mtype == OFPT_MULTIPART_REQUEST || mtype == OFPT_MULTIPART_REPLY) &&
      buf_.size() >= 12) {
    UInt16 flags = *Big16_cast(buf_.data() + 10);
    return static_cast<OFPMultipartFlags>(flags);
  }
  return OFPMPF_NONE;
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

void Message::normalize() {
  Normalize tr{this};
  tr.normalize();
}

void Message::replyError(OFPErrorCode error,
                         const std::string &explanation) const {
  ErrorBuilder errorBuilder{xid()};
  errorBuilder.setErrorCode(error);
  if (!explanation.empty()) {
    errorBuilder.setErrorData(explanation.data(), explanation.size());
  } else {
    errorBuilder.setErrorData(this);
  }
  errorBuilder.send(channel_);
}
