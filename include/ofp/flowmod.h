//  ===== ---- ofp/flowmod.h -------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the FlowMod and FlowModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //
#ifndef OFP_FLOWMOD_H
#define OFP_FLOWMOD_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionlist.h"
#include "ofp/standardmatch.h"

namespace ofp { // <namespace ofp>

class FlowModBuilder;

enum FlowModCommand : UInt8 {
    OFPFC_ADD = 0,
    OFPFC_MODIFY = 1,
    OFPFC_MODIFY_STRICT = 2,
    OFPFC_DELETE = 3,
    OFPFC_DELETE_STRICT = 4
};

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
    FlowModCommand command() const { return static_cast<FlowModCommand>(command_); }
    UInt16 idleTimeout() const { return idleTimeout_; }
    UInt16 hardTimeout() const  { return hardTimeout_; }
    UInt16 priority() const { return priority_; }
    UInt32 bufferId() const { return bufferId_; }
    UInt32 outPort() const { return outPort_; }
    UInt32 outGroup() const { return outGroup_; }
    UInt16 flags() const { return flags_; }

    Match match() const;

    #if 0
    Instructions instructions() const;
    #endif
    
    bool validateLength(size_t length) const;

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

    friend class FlowModBuilder;
    friend struct llvm::yaml::MappingTraits<FlowMod>;
    friend struct llvm::yaml::MappingTraits<FlowModBuilder>;
};

static_assert(sizeof(FlowMod) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowMod>(), "Expected standard layout.");

class FlowModBuilder {
public:
    FlowModBuilder() = default;
    
    void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
    void setCookieMask(UInt64 cookieMask) { msg_.cookieMask_ = cookieMask; }
    void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
    void setCommand(FlowModCommand command) { msg_.command_ = command; }
    void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
    void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
    void setPriority(UInt16 priority) { msg_.priority_ = priority; }
    void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
    void setOutPort(UInt32 outPort) { msg_.outPort_ = outPort; }
    void setOutGroup(UInt32 outGroup) { msg_.outGroup_ = outGroup; }
    void setFlags(UInt16 flags) { msg_.flags_ = flags; }
    
    void setMatch(const MatchBuilder &match) {
        match_ = match;
    }

    void setInstructions(const InstructionList &instructions) {
        instructions_ = instructions;
    }
    
    UInt32 send(Writable *channel);

private:
    FlowMod msg_;
    MatchBuilder match_;
    InstructionList instructions_;

    UInt32 sendStandard(Writable *channel);
    UInt32 sendOriginal(Writable *channel);
    
    friend struct llvm::yaml::MappingTraits<FlowModBuilder>;
};

} // </namespace ofp>

#endif // OFP_FLOWMOD_H
