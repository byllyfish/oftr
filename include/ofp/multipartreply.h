// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MULTIPARTREPLY_H_
#define OFP_MULTIPARTREPLY_H_

#include "ofp/bytelist.h"
#include "ofp/padding.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class MemoryChannel;

class MultipartReply : public ProtocolMsg<MultipartReply, OFPT_MULTIPART_REPLY,
                                          16, 65535, false> {
 public:
  UInt8 version() const { return header_.version(); }

  OFPMultipartType replyType() const { return type_; }
  OFPMultipartFlags replyFlags() const { return flags_; }

  const UInt8 *replyBody() const {
    return BytePtr(this) + sizeof(MultipartReply);
  }

  size_t replyBodySize() const {
    return header_.length() - sizeof(MultipartReply);
  }

  template <class Type>
  const Type *body_cast() const {
    return Interpret_cast<Type>(replyBody());
  }

  bool validateInput(Validation *context) const;

  enum : size_t { UnpaddedSizeVersion1 = 12 };

 private:
  Header header_;
  Big<OFPMultipartType> type_;
  Big<OFPMultipartFlags> flags_;
  Padding<4> pad_;

  // Only MultipartReplyBuilder can construct an instance.
  MultipartReply() : header_{type()} {}

  friend class MultipartReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MultipartReply) == 16, "Unexpected size.");
static_assert(IsStandardLayout<MultipartReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<MultipartReply>(),
              "Expected trivially copyable.");

class MultipartReplyBuilder {
 public:
  static const size_t MAX_BODY_SIZE = OFP_MAX_SIZE - sizeof(MultipartReply);

  MultipartReplyBuilder() = default;
  explicit MultipartReplyBuilder(UInt8 version) {
    msg_.header_.setVersion(version);
  }

  UInt8 version() const { return msg_.header_.version(); }

  void setReplyType(OFPMultipartType type) { msg_.type_ = type; }
  void setReplyFlags(OFPMultipartFlags flags) { msg_.flags_ = flags; }
  void setReplyBody(const void *data, size_t length) {
    body_.set(data, length);
  }

  UInt32 send(Writable *channel);

  // Break reply body into chunks and send each chunk, except the last.
  // Upon return, the last chunk is loaded so client can call send().
  void sendUsingReplyBody(MemoryChannel *channel, const void *data,
                          size_t length, size_t offset);

 private:
  MultipartReply msg_;
  ByteList body_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MULTIPARTREPLY_H_
