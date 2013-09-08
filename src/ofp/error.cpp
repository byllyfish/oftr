#include "ofp/error.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

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
    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(channel->nextXid());

    channel->write(&msg_, sizeof(msg_));
    if (data_.size() > 0) {
        channel->write(data_.data(), data_.size());
    }
    channel->flush();
}

} // </namespace ofp>
