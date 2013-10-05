#include "ofp/groupmod.h"
#include "ofp/writable.h"

using namespace ofp;

bool GroupMod::validateLength(size_t length) const
{
	return false;
}


GroupModBuilder::GroupModBuilder(const GroupMod *msg)
{
	msg_ = *msg;

	// FIXME copy actions
}

UInt32 GroupModBuilder::send(Writable *channel)
{
	UInt32 xid = channel->nextXid();
	size_t msgLen = sizeof(msg_) + buckets_.size();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->write(buckets_.data(), buckets_.size());
    channel->flush();

    return xid;
}
