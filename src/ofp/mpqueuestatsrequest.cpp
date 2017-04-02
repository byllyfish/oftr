// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpqueuestatsrequest.h"
#include "ofp/constants.h"
#include "ofp/multipartrequest.h"
#include "ofp/writable.h"

using namespace ofp;

const MPQueueStatsRequest *MPQueueStatsRequest::cast(
    const MultipartRequest *req) {
  return req->body_cast<MPQueueStatsRequest>();
}

bool MPQueueStatsRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPQueueStatsRequest)) {
    log_debug("MPQueueStatsRequest: Validation failed.");
    return false;
  }

  return true;
}

void MPQueueStatsRequestBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    struct {
      Big16 portNo;
      Padding<2> pad;
    } p;
    p.portNo = UInt16_narrow_cast(msg_.portNo_);

    channel->write(&p, sizeof(p));
    channel->write(&msg_.queueId_, sizeof(msg_.queueId_));

  } else {
    channel->write(&msg_, sizeof(msg_));
  }

  channel->flush();
}
