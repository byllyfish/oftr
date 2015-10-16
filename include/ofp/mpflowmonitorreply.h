// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_MPFLOWMONITORREPLY_H_
#define OFP_MPFLOWMONITORREPLY_H_

#include "ofp/padding.h"
#include "ofp/matchheader.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionlist.h"
#include "ofp/tablenumber.h"

namespace ofp {

class Writable;

class MPFlowMonitorReply {
 public:
  enum { MPVariableSizeOffset = 0 };

  OFPFlowUpdateEvent event() const { return event_; }

  UInt32 xid() const {
    assert(abbrev());
    return *Big32_cast(&tableId_);
  }

  TableNumber tableId() const {
    assert(full());
    return tableId_;
  }

  UInt8 reason() const {
    assert(full());
    return reason_;
  }

  UInt16 idleTimeout() const {
    assert(full());
    return idleTimeout_;
  }

  UInt16 hardTimeout() const {
    assert(full());
    return hardTimeout_;
  }

  UInt16 priority() const {
    assert(full());
    return priority_;
  }

  UInt64 cookie() const {
    assert(full());
    return cookie_;
  }

  Match match() const;
  InstructionRange instructions() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Big<OFPFlowUpdateEvent> event_;
  TableNumber tableId_;
  Big8 reason_;
  Big16 idleTimeout_;
  Big16 hardTimeout_;
  Big16 priority_;
  Padding<4> zeros_;
  Big64 cookie_;
  MatchHeader matchHeader_;
  Padding<4> pad_;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 28,
    SizeWithoutMatchHeader = 24,
    AbbrevSize = 8,
  };

  bool abbrev() const { return length_ == 8 && event_ == OFPFME_ABBREV; }
  bool full() const { return length_ > 8 && event_ <= OFPFME_MODIFIED; }

  friend class MPFlowMonitorReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPFlowMonitorReply) == 32, "Unexpected size.");
static_assert(IsStandardLayout<MPFlowMonitorReply>(),
              "Expected standard layout.");

class MPFlowMonitorReplyBuilder {
 public:
  void setEvent(OFPFlowUpdateEvent event) { msg_.event_ = event; }

  void setXid(UInt32 xid) { *Big32_cast(&msg_.tableId_) = xid; }

  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setReason(UInt8 reason) { msg_.reason_ = reason; }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }

  void setMatch(const MatchBuilder &match) { match_ = match; }
  void setInstructions(const InstructionList &instructions) {
    instructions_ = instructions;
  }

  void write(Writable *channel);
  void reset();

 private:
  MPFlowMonitorReply msg_;
  MatchBuilder match_;
  InstructionList instructions_;

  void writeAbbrev(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPFLOWMONITORREPLY_H_
