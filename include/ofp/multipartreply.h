//  ===== ---- ofp/multipartreply.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines MultipartReply and MultipartReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MULTIPARTREPLY_H_
#define OFP_MULTIPARTREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp {

class MultipartReply
    : public ProtocolMsg<MultipartReply, OFPT_MULTIPART_REPLY, 16, 65528> {
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
    const Type *p = reinterpret_cast<const Type *>(replyBody());
    if (!p->validateInput(replyBodySize())) {
      return nullptr;
    }
    return p;
  }

  bool validateInput(size_t length) const;

  enum : size_t {
    UnpaddedSizeVersion1 = 12
  };

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
