// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MESSAGE_H_
#define OFP_MESSAGE_H_

#include "ofp/bytelist.h"
#include "ofp/datapathid.h"
#include "ofp/header.h"
#include "ofp/messagetype.h"
#include "ofp/timestamp.h"
#include "ofp/validation.h"

namespace ofp {

class Writable;
class Channel;
class MessageInfo;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a protocol message buffer.
class Message {
 public:
  // Flags used (in version field) to indicate pre-processing errors.
  enum : UInt8 {
    kTooBigErrorFlag = 0xE0,
    kInvalidErrorFlag = 0xF0,
  };

  explicit Message(Channel *channel) : channel_{channel} {
    buf_.resize(sizeof(Header));
  }

  Message(const void *data, size_t size) : channel_{nullptr} {
    assert(size >= sizeof(Header));
    buf_.add(data, size);
    assert(header()->length() == size);
  }

  UInt8 *mutableDataResized(size_t size) {
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

  UInt8 getByteAtIndex(size_t offset) const {
    assert(offset < buf_.size());
    return buf_.data()[offset];
  }

  void setByteAtIndex(UInt8 val, size_t offset) {
    assert(offset < buf_.size());
    buf_.mutableData()[offset] = val;
  }

  const Header *header() const { return Interpret_cast<Header>(buf_.data()); }

  Header *mutableHeader() {
    return reinterpret_cast<Header *>(buf_.mutableData());
  }

  void setData(const UInt8 *data, size_t length) { buf_.set(data, length); }
  void setSource(Channel *source) { channel_ = source; }
  void setInfo(MessageInfo *info) { info_ = info; }
  void setTime(const Timestamp &time) { time_ = time; }
  // N.B. This does not set multipart flags...
  void setMsgFlags(OFPMessageFlags msgFlags) { msgFlags_ = msgFlags; }

  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }

  Timestamp time() const { return time_; }
  MessageInfo *info() const { return info_; }
  MessageType msgType() const { return MessageType{type(), subtype()}; }
  OFPType type() const { return header()->type(); }
  OFPMultipartType subtype() const;
  OFPMultipartFlags multipartFlags() const;
  OFPMessageFlags msgFlags() const { return msgFlags_ | multipartFlags(); }
  Channel *source() const { return channel_; }
  UInt32 xid() const { return header()->xid(); }
  UInt8 version() const { return header()->version(); }
  bool isRequestType() const;

  bool isValidHeader();
  void normalize();

  /// Send an error message back to the source of the message.
  void replyError(OFPErrorCode error,
                  const std::string &explanation = "") const;

  void assign(const Message &message) { buf_ = message.buf_.toRange(); }

 private:
  ByteList buf_;
  Channel *channel_;
  Timestamp time_;

  // MessageInfo stores extra information about the message's session (src,
  // dest, filename, etc.) It is *not* owned by the message object.
  MessageInfo *info_ = nullptr;
  OFPMessageFlags msgFlags_ = OFP_DEFAULT_MESSAGE_FLAGS;

  // Used by the ProtocolMsg::cast(message) operator.
  template <class MsgType>
  const MsgType *castMessage(OFPErrorCode *error) const;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const Message &msg) {
    return os << msg.buf_;
  }

  friend class Normalize;

  template <class MsgClass, OFPType MsgType, size_t MsgMinLength,
          size_t MsgMaxLength, bool MsgMultiple8>
  friend class ProtocolMsg;
};

OFP_END_IGNORE_PADDING

// Provides convenient implementation of message cast.
template <class MsgType>
const MsgType *Message::castMessage(OFPErrorCode *error) const {
  assert(type() == MsgType::type());

  size_t length = size();
  assert(length == header()->length());

  Validation context{this, error};

  UInt8 versionFlag = version() & 0xF0;
  if (versionFlag) {
    if (versionFlag == kTooBigErrorFlag)
      context.messagePreprocessTooBigError();
    else if (versionFlag == kInvalidErrorFlag)
      context.messagePreprocessFailure();
    return nullptr;
  }

  if (!MsgType::isLengthValid(length)) {
    context.messageSizeIsInvalid();
    return nullptr;
  }

  const MsgType *msg = Interpret_cast<MsgType>(data());
  if (!msg->validateInput(&context)) {
    return nullptr;
  }

  return msg;
}

}  // namespace ofp

#endif  // OFP_MESSAGE_H_
