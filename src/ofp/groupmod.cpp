#include "ofp/groupmod.h"
#include "ofp/writable.h"

using namespace ofp;


BucketRange GroupMod::buckets() const
{
    size_t len = header_.length();
    assert(len >= sizeof(GroupMod));

    return BucketRange{ByteRange{BytePtr(this) + sizeof(GroupMod), len - sizeof(GroupMod)}};
}

bool GroupMod::validateLength(size_t length) const
{
	if (length < sizeof(GroupMod)) {
        return false;
    }

    return ((length - sizeof(GroupMod)) % 8) == 0;
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
