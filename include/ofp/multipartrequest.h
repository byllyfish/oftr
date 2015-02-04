// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MULTIPARTREQUEST_H_
#define OFP_MULTIPARTREQUEST_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp {

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
    return reinterpret_cast<const Type *>(requestBody());
  }

  template <class Type>
  static const MultipartRequest *msg_cast(const Type *body) {
    return reinterpret_cast<const MultipartRequest *>(BytePtr(body) -
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
    body_.set(data, length);
  }

  UInt32 send(Writable *channel);

 private:
  MultipartRequest msg_;
  ByteList body_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MULTIPARTREQUEST_H_
