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

    FlowMod() : header_{Type} {}

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
};

static_assert(sizeof(FlowMod) == 56, "Unexpected FlowMod size.");

} // </namespace ofp>

#endif // OFP_FLOWMOD_H
