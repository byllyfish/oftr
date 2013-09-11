#include "ofp/echorequest.h"
#include "ofp/message.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

const EchoRequest *EchoRequest::cast(const Message *message)
{
    return message->cast<EchoRequest>();
}

ByteRange EchoRequest::echoData() const
{
    return ByteRange{BytePtr(this) + sizeof(Header),
                     header_.length() - sizeof(Header)};
}

bool EchoRequest::validateLength(size_t length) const
{
    return (length >= sizeof(Header));
}

UInt32 EchoRequestBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    UInt32 xid = channel->nextXid();
    size_t msgLen = sizeof(Header) + data_.size();
    msg_.header_.setVersion(version);
    msg_.header_.setXid(xid);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    channel->write(&msg_, sizeof(msg_));
    channel->write(data_.data(), data_.size());
    channel->flush();

    return xid;
}

} // </namespace ofp>