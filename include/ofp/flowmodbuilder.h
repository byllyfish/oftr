#ifndef OFP_FLOWMODBUILDER_H
#define OFP_FLOWMODBUILDER_H

#include "ofp/flowmod.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionset.h"

namespace ofp { // <namespace ofp>

class FlowModBuilder {
public:

	FlowModBuilder() = default;
	
	void setCookie(UInt64 cookie, UInt64 cookieMask);
	void setTableId(UInt8 tableId);
	void setCommand(UInt8 command);
	void setIdleTimeout(UInt32 idleTimeout);
	void setHardTimeout(UInt32 hardTimeout);
	void setPriority(UInt16 priority);
	void setBufferId(UInt32 bufferId);
	void setOutPort(UInt32 outPort);
	void setOutGroup(UInt32 outGroup);
	void setFlags(UInt16 flags);
	
	void setMatch(const MatchBuilder &match) {
		match_ = match;
	}

	void setInstructions(const InstructionSet &instructions) {
		instructions_ = instructions;
	}
	
	#if 0
	void send(const Channel *channel);
	#endif

	std::string serialize() {
		return "";
	}

private:
	FlowMod msg_;
	MatchBuilder match_;
	InstructionSet instructions_;
};

} // </namespace ofp>

#endif //OFP_FLOWMODBUILDER_H

