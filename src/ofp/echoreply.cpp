#include "ofp/echoreply.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

const EchoReply *EchoReply::cast(const Message *message)
{
    return message->cast<EchoReply>();
}

bool EchoReply::validateLength(size_t length) const
{
	return length >= sizeof(Header);
}

ByteRange EchoReply::echoData() const
{
    return ByteRange{BytePtr(this) + sizeof(Header),
                     header_.length() - sizeof(Header)};
}

EchoReplyBuilder::EchoReplyBuilder(UInt32 xid)
{
	msg_.header_.setXid(xid);
}

void EchoReplyBuilder::send(Writable *channel)
{
	UInt8 version = channel->version();
	size_t msgLen = sizeof(msg_) + data_.size();

	msg_.header_.setVersion(version);
	msg_.header_.setLength(UInt16_narrow_cast(msgLen));

	channel->write(&msg_, sizeof(msg_));
	channel->write(data_.data(), data_.size());
	channel->flush();
}

} // </namespace ofp>