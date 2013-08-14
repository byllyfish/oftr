#ifndef OFP_FLOWMOD_H
#define OFP_FLOWMOD_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/padding.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionset.h"
#include "ofp/standardmatch.h"

namespace ofp { // <namespace ofp>

class FlowMod {
public:
    static const FlowMod *cast(const Message *message);

    enum {
        Type = OFPT_FLOW_MOD
    };

    FlowMod() : header_{Type} {}

    UInt64 cookie() const { return cookie_; }
    UInt64 cookieMask() const { return cookieMask_; }
    UInt8  tableId() const { return tableId_; }
    UInt8  command() const { return command_; }
    UInt16 idleTimeout() const { return idleTimeout_; }
    UInt16 hardTimeout() const  { return hardTimeout_; }
    UInt16 priority() const { return priority_; }
    UInt32 bufferId() const { return bufferId_; }
    UInt32 outPort() const { return outPort_; }
    UInt32 outGroup() const { return outGroup_; }
    UInt16 flags() const { return flags_; }

    #if 0
    Match match() const;
    Instructions instructions() const;
    #endif
    
private:
    Header header_;
    Big64 cookie_ = 0;
    Big64 cookieMask_ = 0;
    Big8 tableId_ = 0;
    Big8 command_ = 0;
    Big16 idleTimeout_ = 0;
    Big16 hardTimeout_ = 0;
    Big16 priority_ = 0;
    Big32 bufferId_ = 0;
    Big32 outPort_ = 0;
    Big32 outGroup_ = 0;
    Big16 flags_ = 0;
    Padding<2> pad_1;

    Big16 matchType_ = 0;
    Big16 matchLength_ = 0;
    Padding<4> pad_2;

    enum { UnpaddedSizeWithMatchHeader = 52 };
    enum { SizeWithoutMatchHeader = 48 };

    bool validateLength(size_t length) const;

    friend class FlowModBuilder;
};

static_assert(sizeof(FlowMod) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowMod>(), "Expected standard layout.");

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
            return sendStandard(channel);
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

    UInt32 sendStandard(Writable *channel)
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

#endif // OFP_FLOWMOD_H
