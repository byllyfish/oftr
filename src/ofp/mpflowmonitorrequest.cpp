// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpflowmonitorrequest.h"

#include "ofp/multipartrequest.h"
#include "ofp/writable.h"

using namespace ofp;

const MPFlowMonitorRequest *MPFlowMonitorRequest::cast(
    const MultipartRequest *req) {
  return req->body_cast<MPFlowMonitorRequest>();
}

bool MPFlowMonitorRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();

  if (length < SizeWithoutMatchHeader) {
    log_debug("MPFlowMonitorRequest: Too short.");
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader, context)) {
    log_debug("MPFlowMonitorRequest: Invalid match.");
    return false;
  }

  if (length != SizeWithoutMatchHeader + matchHeader_.paddedLength()) {
    log_debug("MPFlowMonitorRequest: Invalid length.");
    return false;
  }

  return true;
}

Match MPFlowMonitorRequest::match() const {
  return Match{&matchHeader_};
}

void MPFlowMonitorRequestBuilder::write(Writable *channel) {
  size_t matchLen = sizeof(MatchHeader) + match_.size();
  size_t matchLenPadded = PadLength(matchLen);

  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(matchLen);

  channel->write(&msg_, MPFlowMonitorRequest::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), matchLenPadded - matchLen);
  channel->flush();
}
