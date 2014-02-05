//  ===== ---- ofp/multipartrequest.h ----------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the MultipartRequest and MultipartRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MULTIPARTREQUEST_H_
#define OFP_MULTIPARTREQUEST_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp {

class MultipartRequest
    : public ProtocolMsg<MultipartRequest, OFPT_MULTIPART_REQUEST, 16, 65528> {
public:
  OFPMultipartType requestType() const { return type_; }
  UInt16 requestFlags() const { return flags_; }
  const UInt8 *requestBody() const {
    return BytePtr(this) + sizeof(MultipartRequest);
  }
  size_t requestBodySize() const {
    return header_.length() - sizeof(MultipartRequest);
  }

  bool validateLength(size_t length) const;

  template <class Type>
  const Type *body_cast() const {
    const Type *p = reinterpret_cast<const Type *>(requestBody());
    if (!p->validateLength(requestBodySize())) {
      return nullptr;
    }
    return p;
  }

private:
  Header header_;
  Big<OFPMultipartType> type_;
  Big16 flags_;
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
  void setRequestFlags(UInt16 flags) { msg_.flags_ = flags; }
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
