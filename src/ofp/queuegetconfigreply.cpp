// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queuegetconfigreply.h"

#include "ofp/writable.h"

using namespace ofp;

QueueRange QueueGetConfigReply::queues() const {
  return SafeByteRange(this, header_.length(), sizeof(QueueGetConfigReply));
}

bool QueueGetConfigReply::validateInput(Validation *context) const {
  return queues().validateInput(context);
}

UInt32 QueueGetConfigReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  if (version == OFP_VERSION_1) {
    return sendV1(channel);
  }

  QueueRange queues = queues_.toRange();
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_) + queues.writeSize(channel);

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(msgLen);

  channel->write(&msg_, sizeof(msg_));
  queues.write(channel);
  channel->flush();

  return xid;
}

UInt32 QueueGetConfigReplyBuilder::sendV1(Writable *channel) {
  struct {
    Big16 port;
    Padding<6> pad;
  } reply;

  static_assert(sizeof(reply) == 8, "Unexpected size");

  QueueRange queues = queues_.toRange();
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_) + queues.writeSize(channel);

  msg_.header_.setVersion(OFP_VERSION_1);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(msgLen);
  msg_.header_.setRawType(deprecated::v1::OFPT_QUEUE_GET_CONFIG_REPLY);

  reply.port = msg_.port().toV1();

  channel->write(&msg_.header_, sizeof(msg_.header_));
  channel->write(&reply, sizeof(reply));
  queues.write(channel);

  channel->flush();

  return xid;
}
