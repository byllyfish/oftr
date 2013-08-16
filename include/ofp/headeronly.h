#ifndef OFP_HEADERONLY_H
#define OFP_HEADERONLY_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/writable.h"

namespace ofp {    // <namespace ofp>
namespace detail { // <namespace detail>

template <UInt8 MsgType>
class HeaderOnly {
public:
    /// Cast message to this type after validating contents.
    ///
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

/// Represents an immutable OFPT_FEATURES_REQUEST message.
using FeaturesRequest = detail::HeaderOnly<OFPT_FEATURES_REQUEST>;

/// Builds a mutable OFPT_FEATURES_REQUEST message and delivers it to a channel.
using FeaturesRequestBuilder = detail::HeaderOnlyBuilder<FeaturesRequest>;

static_assert(sizeof(FeaturesRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesRequest>(), "Expected standard layout.");

/// Concrete class that parses an immmutable OFPT_GET_ASYNC_REQUEST
/// message.
using GetAsyncRequest = detail::HeaderOnly<OFPT_GET_ASYNC_REQUEST>;

/// Concrete class that builds a mutable OFPT_GET_ASYNC_REQUEST message
/// and delivers it to a channel.
using GetAsyncRequestBuilder = detail::HeaderOnlyBuilder<GetAsyncRequest>;

static_assert(sizeof(GetAsyncRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncRequest>(), "Expected standard layout.");

/// Concrete class that parses an immutable OFPT_GET_CONFIG_REQUEST message.
using GetConfigRequest = detail::HeaderOnly<OFPT_GET_CONFIG_REQUEST>;

/// Concrete class that builds a mutable OFPT_GET_CONFIG_REQUEST message and
/// delivers it to a channel.
using GetConfigRequestBuilder = detail::HeaderOnlyBuilder<GetConfigRequest>;

static_assert(sizeof(GetConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigRequest>(),
              "Expected standard layout.");

// ^^^^^^^^^^^^^^^^^^^^^^ I M P L E M E N T A T I O N ^^^^^^^^^^^^^^^^^^^^^^^ //

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
