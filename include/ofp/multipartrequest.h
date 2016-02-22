// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MULTIPARTREQUEST_H_
#define OFP_MULTIPARTREQUEST_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp {

class MemoryChannel;

class MultipartRequest
    : public ProtocolMsg<MultipartRequest, OFPT_MULTIPART_REQUEST, 16, 65535,
                         false> {
 public:
  OFPMultipartType requestType() const { return type_; }
  OFPMultipartFlags requestFlags() const { return flags_; }
  const UInt8 *requestBody() const {
    return BytePtr(this) + sizeof(MultipartRequest);
  }
  size_t requestBodySize() const {
    return header_.length() - sizeof(MultipartRequest);
  }

  bool validateInput(Validation *context) const;

  template <class Type>
  const Type *body_cast() const {
    return Interpret_cast<Type>(requestBody());
  }

  template <class Type>
  static const MultipartRequest *msg_cast(const Type *body) {
    return Interpret_cast<MultipartRequest>(BytePtr(body) -
                                            sizeof(MultipartRequest));
  }

  enum : size_t { UnpaddedSizeVersion1 = 12 };

 private:
  Header header_;
  Big<OFPMultipartType> type_;
  Big<OFPMultipartFlags> flags_;
  Padding<4> pad_;

  // Only MultipartRequestBuilder can construct an instance.
  MultipartRequest() : header_{type()} {}

  friend class MultipartRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MultipartRequest) == 16, "Unexpected size.");
static_assert(IsStandardLayout<MultipartRequest>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<MultipartRequest>(),
              "Expected trivially copyable.");

class MultipartRequestBuilder {
 public:
  MultipartRequestBuilder() = default;
  explicit MultipartRequestBuilder(UInt8 version) {
    msg_.header_.setVersion(version);
  }

  UInt8 version() const { return msg_.header_.version(); }

  void setRequestType(OFPMultipartType type) { msg_.type_ = type; }
  void setRequestFlags(OFPMultipartFlags flags) { msg_.flags_ = flags; }
  void setRequestBody(const void *data, size_t length) {
    assert(length <= MAX_BODY_SIZE);
    body_.set(data, length);
  }

  UInt32 send(Writable *channel);

  // Break request body into chunks and send each chunk, except the last.
  // Upon return, the last chunk is loaded so client can call send().
  void sendUsingRequestBody(MemoryChannel *channel, const void *data, size_t length);

 private:
  MultipartRequest msg_;
  ByteList body_;

  static const size_t MAX_BODY_SIZE = OFP_MAX_SIZE - sizeof(msg_);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MULTIPARTREQUEST_H_
