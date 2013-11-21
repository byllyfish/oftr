#include "ofp/mpportstats.h"
#include "ofp/writable.h"
#include "ofp/constants.h"

using namespace ofp;

void MPPortStatsBuilder::write(Writable *channel) {
	UInt8 version = channel->version();

	if (version == OFP_VERSION_1) {

		struct {
			Big16 portNo;
			Padding<6> pad;
		} p;
		p.portNo = UInt16_narrow_cast(msg_.portNo_);

		channel->write(&p, sizeof(p));
		channel->write(&msg_.rxPackets_, 8*12);
		channel->flush();

	} else {
		channel->write(&msg_, sizeof(msg_));
		channel->flush();
	}
}
