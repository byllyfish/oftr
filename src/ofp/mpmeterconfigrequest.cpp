#include "ofp/mpmeterconfigrequest.h"
#include "ofp/multipartrequest.h"
#include "ofp/writable.h"

using namespace ofp;

const MPMeterConfigRequest *MPMeterConfigRequest::cast(const MultipartRequest *req) {
    return req->body_cast<MPMeterConfigRequest>();
}


void MPMeterConfigRequestBuilder::write(Writable *channel) {
    channel->write(&msg_, sizeof(msg_));
    channel->flush();
}
