#ifndef OFP_FLOWMODBUILDER_H
#define OFP_FLOWMODBUILDER_H

#include "ofp/flowmod.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionset.h"
#include "ofp/standardmatch.h"
#include "ofp/writable.h"

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
	
	UInt32 send(Writable *channel) {
		UInt8  version = channel->version();
		if (version <= 0x02) {
			return writeStandard(channel);
		}

		// Calculate length of FlowMod message up to end of match section. Then 
		// pad it to a multiple of 8 bytes.
		size_t msgMatchLen = FlowMod::UnpaddedSizeWithMatchHeader + match_.size();
		size_t msgMatchLenPadded = PadLength(msgMatchLen);

		// Calculate length of instruction section. Then, pad it to a multiple
		// of 8 bytes.
		size_t instrLen = instructions_.size();
		size_t instrLenPadded = PadLength(instrLen);

		// Calculate the total FlowMod message length.
		size_t msgLen = msgMatchLenPadded + instrLenPadded;

		// Fill in the message header.
		UInt32 xid = channel->nextXid();
		Header &hdr = msg_.header_;
		hdr.setVersion(version);
		hdr.setType(FlowMod::Type);
		hdr.setLength(UInt16_narrow_cast(msgLen));
		hdr.setXid(xid);

		// Fill in the match header.
		msg_.matchType_ = OFPMT_OXM;
		msg_.matchLength_ = UInt16_narrow_cast(PadLength(match_.size()));

		// Write the message with padding in the correct spots.
		Padding<8> pad;
		channel->write(&msg_, FlowMod::UnpaddedSizeWithMatchHeader);
		channel->write(match_.data(), match_.size());
		channel->write(&pad, msgMatchLenPadded - msgMatchLen);
		channel->write(instructions_.data(), instructions_.size());

		return xid;
	}

	UInt32 writeStandard(Writable *channel)
	{
		UInt8 version = channel->version();
		assert(version <= 0x02);

		deprecated::StandardMatch stdMatch{match_.toRange()};

		size_t msgMatchLen = FlowMod::SizeWithoutMatchHeader + sizeof(stdMatch);
		size_t instrLen = instructions_.size();
		size_t instrLenPadded = PadLength(instrLen);
		size_t msgLen = msgMatchLen + instrLenPadded;

		UInt32 xid = channel->nextXid();
		Header &hdr = msg_.header_;
		hdr.setVersion(version);
		hdr.setType(FlowMod::Type);
		hdr.setLength(UInt16_narrow_cast(msgLen));
		hdr.setXid(xid);

		channel->write(&msg_, FlowMod::SizeWithoutMatchHeader);
		channel->write(&stdMatch, sizeof(stdMatch));
		channel->write(instructions_.data(), instrLen);

		return xid;
	}

private:
	FlowMod msg_;
	MatchBuilder match_;
	InstructionSet instructions_;
};

} // </namespace ofp>

#endif //OFP_FLOWMODBUILDER_H

