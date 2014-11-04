// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/tablemod.h"
#include "ofp/writable.h"

using namespace ofp;

TableModBuilder::TableModBuilder(const TableMod *msg) : msg_{*msg} {}

UInt32 TableModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(TableMod);

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
