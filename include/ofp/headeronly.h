//  ===== ---- ofp/headeronly.h ----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines message classes containing only the OpenFlow header..
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HEADERONLY_H
#define OFP_HEADERONLY_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/writable.h"

namespace ofp {    // <namespace ofp>
namespace detail { // <namespace detail>

/// \brief Used to implement header-only messages.
template <UInt8 MsgType>
class HeaderOnly {
public:
    /// Cast message to this type after validating contents.
    /// \returns pointer to message or nullptr if not valid.
    static const HeaderOnly *cast(const Message *message);

    enum {
        Type = MsgType
    };

    HeaderOnly() : header_{Type}
    {
    }

    /// \returns true if message length matches length in header.
    bool validateLength(size_t length) const
    {
        return length == sizeof(HeaderOnly);
    }

private:
    Header header_;
};

template <class HeaderOnlyType>
class HeaderOnlyBuilder {
public:
    HeaderOnlyBuilder() = default;

    /// \returns xid assigned to sent message.
    UInt32 send(Writable *channel);

private:
    HeaderOnlyType msg_;
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
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::FeaturesRequest
/// \returns xid assigned to sent message.
using FeaturesRequest = detail::HeaderOnly<OFPT_FEATURES_REQUEST>;

/// @class ofp::FeaturesRequestBuilder
/// @brief Builds a mutable OFPT_FEATURES_REQUEST message and delivers it to a
/// channel.
///
/// @fn UInt32 send(Writable *channel);
/// @memberof ofp::FeaturesRequestBuilder
/// @returns xid assigned to sent message.
using FeaturesRequestBuilder = detail::HeaderOnlyBuilder<FeaturesRequest>;

static_assert(sizeof(FeaturesRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesRequest>(), "Expected standard layout.");

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
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::GetAsyncRequest
/// \returns xid assigned to sent message.
using GetAsyncRequest = detail::HeaderOnly<OFPT_GET_ASYNC_REQUEST>;

/// @class ofp::GetAsyncRequestBuilder
/// @brief Builds a mutable OFPT_GET_ASYNC_REQUEST message and delivers it to a
/// channel.
///
/// @fn UInt32 send(Writable *channel);
/// @memberof ofp::GetAsyncRequestBuilder
/// @returns xid assigned to sent message.
using GetAsyncRequestBuilder = detail::HeaderOnlyBuilder<GetAsyncRequest>;

static_assert(sizeof(GetAsyncRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncRequest>(), "Expected standard layout.");

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
///
/// \fn UInt32 send(Writable *channel);
/// \memberof ofp::GetConfigRequest
/// \returns xid assigned to sent message.
using GetConfigRequest = detail::HeaderOnly<OFPT_GET_CONFIG_REQUEST>;

/// @class ofp::GetConfigRequestBuilder
/// @brief Builds a mutable OFPT_GET_CONFIG_REQUEST message and delivers it to a
/// channel.
///
/// @fn UInt32 send(Writable *channel);
/// @memberof ofp::GetConfigRequestBuilder
/// @returns xid assigned to sent message.
using GetConfigRequestBuilder = detail::HeaderOnlyBuilder<GetConfigRequest>;

static_assert(sizeof(GetConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigRequest>(),
              "Expected standard layout.");

namespace detail { // <namespace detail>

template <UInt8 MsgType>
const HeaderOnly<MsgType> *HeaderOnly<MsgType>::cast(const Message *message)
{
    assert(message->type() == MsgType);

    const HeaderOnly *msg =
        reinterpret_cast<const HeaderOnly *>(message->data());

    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}

template <class HeaderOnlyType>
UInt32 HeaderOnlyBuilder<HeaderOnlyType>::send(Writable *channel)
{
    UInt32 xid = channel->nextXid();

    Header *header = reinterpret_cast<Header *>(&msg_);
    header->setVersion(channel->version());
    header->setLength(sizeof(msg_));
    header->setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}

} // </namespace detail>
} // </namespace ofp>

#endif // OFP_HEADERONLY_H
