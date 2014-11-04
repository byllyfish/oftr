// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpmeterconfigrequest.h"
#include "ofp/multipartrequest.h"
#include "ofp/writable.h"

using namespace ofp;

const MPMeterConfigRequest *MPMeterConfigRequest::cast(
    const MultipartRequest *req) {
  return req->body_cast<MPMeterConfigRequest>();
}

bool MPMeterConfigRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  return (length == sizeof(MPMeterConfigRequest));
}

void MPMeterConfigRequestBuilder::write(Writable *channel) {
  channel->write(&msg_, sizeof(msg_));
  channel->flush();
}
