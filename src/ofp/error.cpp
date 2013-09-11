#include "ofp/error.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

const Error *Error::cast(const Message *message)
{
    return message->cast<Error>();
}

bool Error::validateLength(size_t length) const
{
    return (length >= sizeof(Error));
}

ByteRange Error::errorData() const
{
    return ByteRange{BytePtr(this) + sizeof(Error),
                     header_.length() - sizeof(Error)};
}

ErrorBuilder::ErrorBuilder(UInt16 type, UInt16 code)
{
    msg_.type_ = type;
    msg_.code_ = code;
}

void ErrorBuilder::setErrorData(const Message *message)
{
    assert(message);
    size_t len = message->size();
    if (len > 64)
        len = 64;
    setErrorData(message->data(), len);
}

void ErrorBuilder::send(Writable *channel)
{
    size_t msgLen = sizeof(msg_) + data_.size();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));
    msg_.header_.setXid(channel->nextXid());

    channel->write(&msg_, sizeof(msg_));
    channel->write(data_.data(), data_.size());
    channel->flush();
}

} // </namespace ofp>
