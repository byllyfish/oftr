// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_FLOWREMOVEDV6_H_
#define OFP_FLOWREMOVEDV6_H_

#include "ofp/durationsec.h"
#include "ofp/matchbuilder.h"
#include "ofp/padding.h"
#include "ofp/protocolmsg.h"
#include "ofp/statbuilder.h"
#include "ofp/tablenumber.h"

namespace ofp {

class FlowRemovedV6
    : public ProtocolMsg<FlowRemovedV6, OFPT_FLOW_REMOVED, 56, 65528, true> {
 public:
  UInt64 cookie() const { return cookie_; }
  UInt16 priority() const { return priority_; }
  OFPFlowRemovedReason reason() const { return reason_; }
  TableNumber tableId() const { return tableId_; }
  DurationSec duration() const;
  UInt16 idleTimeout() const { return idleTimeout_; }
  UInt16 hardTimeout() const { return hardTimeout_; }
  UInt64 packetCount() const;
  UInt64 byteCount() const;

  Match match() const;
  Stat stat() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  TableNumber tableId_;
  Big<OFPFlowRemovedReason> reason_;
  Big16 priority_;
  Big16 idleTimeout_;
  Big16 hardTimeout_;
  Big64 cookie_;
  MatchHeader matchHeader_;
  Padding<4> pad_2;

  // Only FlowRemovedV6Builder can create an instance.
  FlowRemovedV6() : header_{type()} {}

  const StatHeader *statHeader() const;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 28,
    SizeWithoutMatchHeader = 24
  };

  friend class FlowRemovedV6Builder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(FlowRemovedV6) == 32, "Unexpected size.");
static_assert(IsStandardLayout<FlowRemovedV6>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FlowRemovedV6>(),
              "Expected trivially copyable.");

class FlowRemovedV6Builder {
 public:
  FlowRemovedV6Builder() = default;
  explicit FlowRemovedV6Builder(const FlowRemovedV6 *msg);

  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setReason(OFPFlowRemovedReason reason) { msg_.reason_ = reason; }
  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setDuration(const DurationSec &duration);
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setPacketCount(UInt64 packetCount);
  void setByteCount(UInt64 byteCount);

  void setMatch(const MatchBuilder &match) { match_ = match; }
  void setStat(const StatBuilder &stat) { stat_ = stat; }

  UInt32 send(Writable *channel);

 private:
  FlowRemovedV6 msg_;
  MatchBuilder match_;
  StatBuilder stat_;

  UInt32 sendStandard(Writable *channel);
  UInt32 sendOriginal(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_FLOWREMOVEDV6_H_
