// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rawmessage.h"
#include "ofp/writable.h"

using namespace ofp;

UInt32 RawMessageBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  size_t msgLen = sizeof(header_) + data_.size();

  header_.setVersion(version);
  header_.setLength(msgLen);

  channel->write(&header_, sizeof(header_));
  channel->write(data_.data(), data_.size());
  channel->flush();

  return header_.xid();
}
