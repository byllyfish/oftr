#include "ofp/queuegetconfigreply.h"
#include "ofp/writable.h"

using namespace ofp;

bool QueueGetConfigReply::validateInput(size_t length) const {
  // TODO(bfish) implement correctly...
  return length == sizeof(QueueGetConfigReply);
}


UInt32 QueueGetConfigReplyBuilder::send(Writable *channel) {
    UInt8 version = channel->version();
    UInt32 xid = channel->nextXid();
    size_t msgLen = sizeof(msg_) + queues_.size();

    msg_.header_.setVersion(version);
    msg_.header_.setXid(xid);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    channel->write(&msg_, sizeof(msg_));
    channel->write(queues_.data(), queues_.size());
    channel->flush();
    
    return xid;
}
