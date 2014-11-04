// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MULTIPARTREPLY_H_
#define OFP_MULTIPARTREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp {

class MultipartReply : public ProtocolMsg<MultipartReply, OFPT_MULTIPART_REPLY,
                                          16, 65535, false> {
 public:
  OFPMultipartType replyType() const { return type_; }
  UInt16 replyFlags() const { return flags_; }

  const UInt8 *replyBody() const {
    return BytePtr(this) + sizeof(MultipartReply);
  }

  size_t replyBodySize() const {
    return header_.length() - sizeof(MultipartReply);
  }

  template <class Type>
  const Type *body_cast() const {
    return reinterpret_cast<const Type *>(replyBody());
  }

  bool validateInput(Validation *context) const;

  enum : size_t { UnpaddedSizeVersion1 = 12 };

 private:
  Header header_;
  Big<OFPMultipartType> type_;
  Big16 flags_;
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
  MultipartReplyBuilder() = default;
  explicit MultipartReplyBuilder(UInt8 version) {
    msg_.header_.setVersion(version);
  }

  UInt8 version() const { return msg_.header_.version(); }

  void setReplyType(OFPMultipartType type) { msg_.type_ = type; }
  void setReplyFlags(UInt16 flags) { msg_.flags_ = flags; }
  void setReplyBody(const void *data, size_t length) {
    body_.set(data, length);
  }

  UInt32 send(Writable *channel);

 private:
  MultipartReply msg_;
  ByteList body_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MULTIPARTREPLY_H_
