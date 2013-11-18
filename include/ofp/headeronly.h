//  ===== ---- ofp/headeronly.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines message classes containing only the OpenFlow header:
/// OFPT_FEATURES_REQUEST, OFPT_GET_ASYNC_REQUEST, OFPT_GET_CONFIG_REQUEST,
/// OFPT_BARRIER_REQUEST, OFPT_BARRIER_REPLY.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HEADERONLY_H
#define OFP_HEADERONLY_H

#include "ofp/header.h"
#include "ofp/writable.h"
#include "ofp/message.h"
#include "ofp/padding.h"
#include "ofp/log.h"

namespace ofp {    // <namespace ofp>
namespace detail { // <namespace detail>

template <class HeaderOnlyType>
class HeaderOnlyBuilder;

/// \brief Used to implement header-only messages.
template <OFPType MsgType>
class HeaderOnly {
public:
  enum : size_t {
    MinLength = 8,    // Minimum length
    MaxLength = 8,    // Maximum length
    Multiple8 = false // Multiple of 8 (don't need to check)
  };

  /// Cast message to this type after validating contents.
  /// \returns pointer to message or nullptr if not valid.
  static const HeaderOnly *cast(const Message *message);

  static constexpr OFPType type() { return MsgType; }

  /// \returns xid in the header.
  UInt32 xid() const { return header_.xid(); }

  /// \returns true if message length matches length in header.
  bool validateLength(size_t length) const {
    return length == sizeof(HeaderOnly);
  }

private:
  Header header_;

  // Only HeaderOnlyBuilder can construct an actual instance.
  HeaderOnly() : header_{type()} {}

  friend class HeaderOnlyBuilder<HeaderOnly>;
};

template <class HeaderOnlyType>
class HeaderOnlyBuilder {
public:
  HeaderOnlyBuilder() = default;

  explicit HeaderOnlyBuilder(const Message *request) : isReply_{true} {
    msg_.header_.setXid(request->xid());
  }

  explicit HeaderOnlyBuilder(const HeaderOnlyType *msg)
      : msg_{*msg}, isReply_{true} {}

  /// \returns xid assigned to sent message.
  UInt32 send(Writable *channel);

private:
  HeaderOnlyType msg_;
  bool isReply_ = false;
  Padding<3> pad_;
};

} // </namespace detail>

/// \class ofp::FeaturesRequest
/// \brief Represents an immutable OFPT_FEATURES_REQUEST message.
/// This request contains the header only.
///
/// \enum { Type = OFPT_FEATURES_REQUEST };
/// \memberof ofp::FeaturesRequest
///
/// \fn bool validateLength(size_t length) const
/// \memberof ofp::FeaturesRequest
/// \returns true if message length matches length in header.

using FeaturesRequest = detail::HeaderOnly<OFPT_FEATURES_REQUEST>;

/// \class ofp::FeaturesRequestBuilder
/// \brief Builds a mutable OFPT_FEATURES_REQUEST message.
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::FeaturesRequestBuilder
/// \returns xid assigned to sent message.

using FeaturesRequestBuilder = detail::HeaderOnlyBuilder<FeaturesRequest>;

static_assert(sizeof(FeaturesRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FeaturesRequest>(),
              "Expected trivially copyable.");

/// \class ofp::GetAsyncRequest
/// \brief Represents an immutable OFPT_GET_ASYNC_REQUEST message.
/// This request contains the header only.
///
/// \enum { Type = OFPT_GET_ASYNC_REQUEST };
/// \memberof ofp::GetAsyncRequest
///
/// \fn bool validateLength(size_t length) const
/// \memberof ofp::GetAsyncRequest
/// \returns true if message length matches length in header.

using GetAsyncRequest = detail::HeaderOnly<OFPT_GET_ASYNC_REQUEST>;

/// \class ofp::GetAsyncRequestBuilder
/// \brief Builds a mutable OFPT_GET_ASYNC_REQUEST message.
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::GetAsyncRequestBuilder
/// \returns xid assigned to sent message.

using GetAsyncRequestBuilder = detail::HeaderOnlyBuilder<GetAsyncRequest>;

static_assert(sizeof(GetAsyncRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetAsyncRequest>(),
              "Expected trivially copyable.");

/// \class ofp::GetConfigRequest
/// \brief Represents an immutable OFPT_GET_CONFIG_REQUEST message.
/// This request contains the header only.
///
/// \enum { Type = OFPT_GET_CONFIG_REQUEST };
/// \memberof ofp::GetConfigRequest
///
/// \fn bool validateLength(size_t length) const
/// \memberof ofp::GetConfigRequest
/// \returns true if message length matches length in header.

using GetConfigRequest = detail::HeaderOnly<OFPT_GET_CONFIG_REQUEST>;

/// \class ofp::GetConfigRequestBuilder
/// \brief Builds a mutable OFPT_GET_CONFIG_REQUEST message.
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::GetConfigRequestBuilder
/// \returns xid assigned to sent message.

using GetConfigRequestBuilder = detail::HeaderOnlyBuilder<GetConfigRequest>;

static_assert(sizeof(GetConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigRequest>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetConfigRequest>(),
              "Expected trivially copyable.");

/// \class ofp::BarrierRequest
/// \brief Represents an immutable OFPT_BARRIER_REQUEST message.
/// This request contains the header only.
///
/// \enum { Type = OFPT_BARRIER_REQUEST };
/// \memberof ofp::BarrierRequest
///
/// \fn bool validateLength(size_t length) const
/// \memberof ofp::BarrierRequest
/// \returns true if message length matches length in header.

using BarrierRequest = detail::HeaderOnly<OFPT_BARRIER_REQUEST>;

/// \class ofp::BarrierRequestBuilder
/// \brief Builds a mutable OFPT_BARRIER_REQUEST message.
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::BarrierRequestBuilder
/// \returns xid assigned to sent message.

using BarrierRequestBuilder = detail::HeaderOnlyBuilder<BarrierRequest>;

static_assert(sizeof(BarrierRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<BarrierRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<BarrierRequest>(),
              "Expected trivially copyable.");

/// \class ofp::BarrierReply
/// \brief Represents an immutable OFPT_BARRIER_REPLY message.
/// This request contains the header only.
///
/// \enum { Type = OFPT_BARRIER_REPLY };
/// \memberof ofp::BarrierReply
///
/// \fn bool validateLength(size_t length) const
/// \memberof ofp::BarrierReply

using BarrierReply = detail::HeaderOnly<OFPT_BARRIER_REPLY>;

/// \class ofp::BarrierReplyBuilder
/// \brief Builds a mutable OFPT_BARRIER_REPLY message.
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::BarrierReplyBuilder
/// \returns xid assigned to sent message.

using BarrierReplyBuilder = detail::HeaderOnlyBuilder<BarrierReply>;

static_assert(sizeof(BarrierReply) == 8, "Unexpected size.");
static_assert(IsStandardLayout<BarrierReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<BarrierReply>(),
              "Expected trivially copyable.");

namespace detail { // <namespace detail>

template <OFPType MsgType>
const HeaderOnly<MsgType> *HeaderOnly<MsgType>::cast(const Message *message) {
  assert(message->type() == MsgType);

  const HeaderOnly *msg = reinterpret_cast<const HeaderOnly *>(message->data());

  if (!msg->validateLength(message->size())) {
    return nullptr;
  }

  return msg;
}

template <class HeaderOnlyType>
UInt32 HeaderOnlyBuilder<HeaderOnlyType>::send(Writable *channel) {
  Header *header = reinterpret_cast<Header *>(&msg_);
  UInt32 xid = isReply_ ? header->xid() : channel->nextXid();

  OFPType origType = header->type();
  UInt8 version = channel->version();
  OFPType newType = Header::translateType(OFP_VERSION_4, origType, version);

  if (newType != OFPT_UNSUPPORTED) {
    header->setType(newType);
    header->setVersion(version);
    header->setLength(sizeof(msg_));
    if (!isReply_)
      header->setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    // Restore original type in case Send() is called twice for same builder.
    header->setType(origType);

  } else {
    log::info("Message not supported on this channel version.", origType);
  }

  return xid;
}

} // </namespace detail>
} // </namespace ofp>

#endif // OFP_HEADERONLY_H
