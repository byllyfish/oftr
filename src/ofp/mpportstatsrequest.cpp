// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpportstatsrequest.h"
#include "ofp/writable.h"
#include "ofp/constants.h"
#include "ofp/multipartrequest.h"

using ofp::MPPortStatsRequest;
using ofp::MPPortStatsRequestBuilder;

const MPPortStatsRequest *MPPortStatsRequest::cast(
    const MultipartRequest *req) {
  return req->body_cast<MPPortStatsRequest>();
}

bool MPPortStatsRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPPortStatsRequest)) {
    log::debug("MPPortStatsRequest: Validation failed.");
    return false;
  }

  return true;
}

void MPPortStatsRequestBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    struct {
      Big16 portNo;
      Padding<6> pad;
    } p;
    p.portNo = UInt16_narrow_cast(msg_.portNo_);

    channel->write(&p, sizeof(p));
  } else {
    channel->write(&msg_, sizeof(msg_));
  }

  channel->flush();
}
