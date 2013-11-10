#ifndef OFP_MPQUEUESTATS_H
#define OFP_MPQUEUESTATS_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

class Writable;

class MPQueueStats {
public:
	MPQueueStats() = default;


private:
	Big32 portNo_;
	Big32 queueId_;
	Big64 txBytes_;
	Big64 txPackets_;
	Big64 txErrors_;
	Big32 durationSec_;
	Big32 durationNSec_;

	friend class MPQueueStatsBuilder;
	template <class T>
	friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPQueueStats) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPQueueStats>(), "Expected standard layout.");

class MPQueueStatsBuilder {
public:

	void write(Writable *channel);

private:
	MPQueueStats msg_;

	template <class T>
	friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_MPQUEUESTATS_H
