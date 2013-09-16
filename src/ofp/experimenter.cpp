#include "ofp/experimenter.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

const Experimenter *Experimenter::cast(const Message *message)
{
    return message->cast<Experimenter>();
}

ByteRange Experimenter::expData() const
{
    return ByteRange{BytePtr(this) + sizeof(Experimenter),
                     header_.length() - sizeof(Experimenter)};
}


bool Experimenter::validateLength(size_t length) const
{
	return length >= sizeof(Experimenter);
}


UInt32 ExperimenterBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    UInt32 xid = channel->nextXid();
    size_t msgLen = sizeof(msg_) + data_.size();

    if (version == OFP_VERSION_1)
        msgLen -= 4;

    msg_.header_.setVersion(version);
    msg_.header_.setXid(xid);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    if (version == OFP_VERSION_1) {
        // Header + experimenter (12 bytes)
        channel->write(&msg_, 12);
    } else if (version < OFP_VERSION_4) {
        // Header + experimenter + 4 byte pad (12 + 4 bytes)
        Padding<4> pad;
        channel->write(&msg_, 12);
        channel->write(&pad, 4);

    } else {
        // Header + experimenter + expType (16 bytes)
        channel->write(&msg_, sizeof(msg_));
    }

    channel->write(data_.data(), data_.size());
    channel->flush();

    return xid;
}

} // </namespace ofp>