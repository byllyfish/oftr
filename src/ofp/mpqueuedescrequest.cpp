#include "ofp/mpqueuedescrequest.h"
#include "ofp/multipartrequest.h"
#include "ofp/writable.h"

using namespace ofp;

const MPQueueDescRequest *MPQueueDescRequest::cast(const MultipartRequest *req) {
  log::debug("cast to MPQueueDescRequest");
  return req->body_cast<MPQueueDescRequest>();
}

bool MPQueueDescRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPQueueDescRequest)) {
    log::debug("MPQueueDescRequest: Validation failed.");
    return false;
  }

  return true;
}

void MPQueueDescRequestBuilder::write(Writable *channel) {
  channel->write(&msg_, sizeof(msg_));
  channel->flush();
}
