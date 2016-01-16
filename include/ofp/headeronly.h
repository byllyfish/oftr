// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_HEADERONLY_H_
#define OFP_HEADERONLY_H_

#include "ofp/header.h"
#include "ofp/writable.h"
#include "ofp/message.h"
#include "ofp/padding.h"
#include "ofp/log.h"

namespace ofp {
namespace detail {

template <class HeaderOnlyType>
class HeaderOnlyBuilder;

/// \brief Used to implement header-only messages.
template <OFPType MsgType>
class HeaderOnly {
 public:
  enum : size_t {
    MinLength = 8,     // Minimum length
    MaxLength = 8,     // Maximum length
    Multiple8 = false  // Multiple of 8 (don't need to check)
  };

  /// Cast message to this type after validating contents.
  /// \returns pointer to message or nullptr if not valid.
  static const HeaderOnly *cast(const Message *message);

  static constexpr OFPType type() { return MsgType; }

  static bool isLengthValid(size_t length) { return length == MinLength; }

  /// \returns xid in the header.
  UInt32 xid() const { return header_.xid(); }

  /// \returns true if message content is valid.
  bool validateInput(Validation *context) const { return true; }

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

}  // namespace detail

/// \typedef ofp::FeaturesRequest
/// \brief Represents an immutable OFPT_FEATURES_REQUEST message.
/// This request contains the header only.

using FeaturesRequest = detail::HeaderOnly<OFPT_FEATURES_REQUEST>;

/// \typedef ofp::FeaturesRequestBuilder
/// \brief Builds a mutable OFPT_FEATURES_REQUEST message.

using FeaturesRequestBuilder = detail::HeaderOnlyBuilder<FeaturesRequest>;

static_assert(sizeof(FeaturesRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FeaturesRequest>(),
              "Expected trivially copyable.");

/// \typedef ofp::GetAsyncRequest
/// \brief Represents an immutable OFPT_GET_ASYNC_REQUEST message.
/// This request contains the header only.

using GetAsyncRequest = detail::HeaderOnly<OFPT_GET_ASYNC_REQUEST>;

/// \typedef ofp::GetAsyncRequestBuilder
/// \brief Builds a mutable OFPT_GET_ASYNC_REQUEST message.

using GetAsyncRequestBuilder = detail::HeaderOnlyBuilder<GetAsyncRequest>;

static_assert(sizeof(GetAsyncRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetAsyncRequest>(),
              "Expected trivially copyable.");

/// \typedef ofp::GetConfigRequest
/// \brief Represents an immutable OFPT_GET_CONFIG_REQUEST message.
/// This request contains the header only.

using GetConfigRequest = detail::HeaderOnly<OFPT_GET_CONFIG_REQUEST>;

/// \typedef ofp::GetConfigRequestBuilder
/// \brief Builds a mutable OFPT_GET_CONFIG_REQUEST message.

using GetConfigRequestBuilder = detail::HeaderOnlyBuilder<GetConfigRequest>;

static_assert(sizeof(GetConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigRequest>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetConfigRequest>(),
              "Expected trivially copyable.");

/// \typedef ofp::BarrierRequest
/// \brief Represents an immutable OFPT_BARRIER_REQUEST message.
/// This request contains the header only.

using BarrierRequest = detail::HeaderOnly<OFPT_BARRIER_REQUEST>;

/// \typedef ofp::BarrierRequestBuilder
/// \brief Builds a mutable OFPT_BARRIER_REQUEST message.

using BarrierRequestBuilder = detail::HeaderOnlyBuilder<BarrierRequest>;

static_assert(sizeof(BarrierRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<BarrierRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<BarrierRequest>(),
              "Expected trivially copyable.");

/// \typedef ofp::BarrierReply
/// \brief Represents an immutable OFPT_BARRIER_REPLY message.
/// This request contains the header only.

using BarrierReply = detail::HeaderOnly<OFPT_BARRIER_REPLY>;

/// \typedef ofp::BarrierReplyBuilder
/// \brief Builds a mutable OFPT_BARRIER_REPLY message.

using BarrierReplyBuilder = detail::HeaderOnlyBuilder<BarrierReply>;

static_assert(sizeof(BarrierReply) == 8, "Unexpected size.");
static_assert(IsStandardLayout<BarrierReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<BarrierReply>(),
              "Expected trivially copyable.");

namespace detail {

template <OFPType MsgType>
const HeaderOnly<MsgType> *HeaderOnly<MsgType>::cast(const Message *message) {
  assert(message->type() == MsgType);

  const HeaderOnly *msg = Interpret_cast<HeaderOnly>(message->data());

  if (message->size() != 8) {
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

}  // namespace detail
}  // namespace ofp

#endif  // OFP_HEADERONLY_H_
