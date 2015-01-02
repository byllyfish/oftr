// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MESSAGE_H_
#define OFP_MESSAGE_H_

#include "ofp/bytelist.h"
#include "ofp/header.h"
#include "ofp/datapathid.h"
#include "ofp/validation.h"
#include "ofp/timestamp.h"

namespace ofp {

class Writable;
class Channel;

namespace sys {
class Connection;
}  // namespace sys

/// \brief Implements a protocol message buffer.
class Message {
 public:
  explicit Message(sys::Connection *channel) : channel_{channel} {
    buf_.resize(sizeof(Header));
  }

  Message(const void *data, size_t size) : channel_{nullptr} {
    assert(size >= sizeof(Header));
    buf_.add(data, size);
    assert(header()->length() == size);
  }

  UInt8 *mutableData(size_t size) {
    buf_.resize(size);
    return buf_.mutableData();
  }

  void shrink(size_t size) {
    assert(size <= buf_.size());
    buf_.resize(size);
  }

  void removeFront(size_t bytes) {
    assert(bytes <= buf_.size());
    buf_.remove(buf_.data(), bytes);
  }

  const Header *header() const {
    return reinterpret_cast<const Header *>(buf_.data());
  }

  Header *mutableHeader() {
    return reinterpret_cast<Header *>(buf_.mutableData());
  }

  void setData(const UInt8 *data, size_t length) { buf_.set(data, length); }
  void setSource(sys::Connection *source) { channel_ = source; }
  void setTime(const Timestamp &time) { time_ = time; }

  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }

  Timestamp time() const { return time_; }
  OFPType type() const { return header()->type(); }
  Channel *source() const;
  UInt32 xid() const { return header()->xid(); }
  UInt8 version() const { return header()->version(); }
  bool isRequestType() const;

  bool isValidHeader();
  void transmogrify();

  // Used by the ProtocolMsg::cast(message) operator.
  template <class MsgType>
  const MsgType *castMessage() const;

  void assign(const Message &message) { buf_ = message.buf_.toRange(); }

 private:
  ByteList buf_;
  sys::Connection *channel_;
  Timestamp time_;

  friend std::ostream &operator<<(std::ostream &os, const Message &msg);
  friend class Transmogrify;
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

inline std::ostream &operator<<(std::ostream &os, const Message &msg) {
  return os << msg.buf_;
}

// Provides convenient implementation of message cast.
template <class MsgType>
const MsgType *Message::castMessage() const {
  assert(type() == MsgType::type());

  size_t length = size();
  assert(length == header()->length());

  Validation context{this};

  if (!MsgType::isLengthValid(length)) {
    context.messageSizeIsInvalid();
    return nullptr;
  }

  const MsgType *msg = reinterpret_cast<const MsgType *>(data());
  if (!msg->validateInput(&context)) {
    return nullptr;
  }

  return msg;
}

}  // namespace ofp

#endif  // OFP_MESSAGE_H_
