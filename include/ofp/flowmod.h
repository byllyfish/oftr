#ifndef OFP_FLOWMOD_H
#define OFP_FLOWMOD_H

#include "ofp/header.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class FlowMod {
public:
    enum {
        Type = OFPT_FLOW_MOD
    };

    const Header *header() const { return &header_; }

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
    Big64 cookie_;
    Big64 cookieMask_;
    Big8 tableId_;
    Big8 command_;
    Big16 idleTimeout_;
    Big16 hardTimeout_;
    Big16 priority_;
    Big32 bufferId_;
    Big32 outPort_;
    Big32 outGroup_;
    Big16 flags_;
    Padding<2> pad_;

    friend class FlowModBuilder;
    //MatchRef match_;
    //InstructionListRef instructions_;
};

} // </namespace ofp>

#endif // OFP_FLOWMOD_H
