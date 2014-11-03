#include "ofp/queuegetconfigreply.h"
#include "ofp/writable.h"

using namespace ofp;

QueueRange QueueGetConfigReply::queues() const {
  return ByteRange{BytePtr(this) + sizeof(QueueGetConfigReply),
                   header_.length() - sizeof(QueueGetConfigReply)};
}

bool QueueGetConfigReply::validateInput(Validation *context) const {
  if (!queues().validateInput(context)) {
    return false;
  }

  return true;
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
