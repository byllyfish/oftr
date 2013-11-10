#include "ofp/mpqueuestats.h"
#include "ofp/writable.h"
#include "ofp/constants.h"
#include "ofp/padding.h"

using namespace ofp;

void MPQueueStatsBuilder::write(Writable *channel)
{
	UInt8 version = channel->version();

	if (version == OFP_VERSION_1) {
		Padding<2> pad;
		channel->write(BytePtr(&msg_.portNo_) + 2, 2);
		channel->write(&pad, sizeof(pad));
		channel->write(&msg_.queueId_, 28);
		channel->flush();

	} else {
		channel->write(&msg_, sizeof(msg_));
		channel->flush();
	}
}