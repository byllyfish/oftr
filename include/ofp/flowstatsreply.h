#ifndef OFP_FLOWSTATSREPLY_H
#define OFP_FLOWSTATSREPLY_H

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/instructionrange.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionlist.h"

namespace ofp { // <namespace ofp>

class Writable;
class FlowStatsReplyBuilder;

class FlowStatsReply {
public:

	FlowStatsReply() = default;

	Match match() const;
	InstructionRange instructions() const;

private:
	Big16 length_;
	Big8 tableId_;
	Padding<1> pad_1;
	Big32 durationSec_;
	Big32 durationNsec_;
	Big16 priority_;
	Big16 idleTimeout_;
	Big16 hardTimeout_;
	Big16 flags_;
	Padding<4> pad_2;
	Big64 cookie_;
	Big64 packetCount_;
	Big64 byteCount_;

	Big16 matchType_ = 0;
    Big16 matchLength_ = 0;
    Padding<4> pad_3;

    enum { UnpaddedSizeWithMatchHeader = 52 };
    enum { SizeWithoutMatchHeader = 48 };

    friend class FlowStatsReplyBuilder;
    friend struct llvm::yaml::MappingTraits<FlowStatsReply>;
    friend struct llvm::yaml::MappingTraits<FlowStatsReplyBuilder>;
};

static_assert(sizeof(FlowStatsReply) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowStatsReply>(), "Expected standard layout.");


class FlowStatsReplyBuilder {
public:
	

	void write(Writable *channel);

private:
	FlowStatsReply msg_;
	MatchBuilder match_;
	InstructionList instructions_;

	friend struct llvm::yaml::MappingTraits<FlowStatsReplyBuilder>;
};

} // </namespace ofp>

#endif // OFP_FLOWSTATSREPLY_H
