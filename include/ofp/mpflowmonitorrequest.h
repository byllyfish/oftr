// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPFLOWMONITORREQUEST_H_
#define OFP_MPFLOWMONITORREQUEST_H_

#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/matchheader.h"
#include "ofp/matchbuilder.h"
#include "ofp/tablenumber.h"
#include "ofp/groupnumber.h"

namespace ofp {

class Writable;
class Validation;
class MultipartRequest;

class MPFlowMonitorRequest {
 public:
  static const MPFlowMonitorRequest *cast(const MultipartRequest *req);

  UInt32 monitorId() const { return monitorId_; }
  PortNumber outPort() const { return outPort_; }
  GroupNumber outGroup() const { return outGroup_; }
  OFPFlowMonitorFlags flags() const { return flags_; }
  TableNumber tableId() const { return tableId_; }
  OFPFlowMonitorCommand command() const { return command_; }

  Match match() const;

  bool validateInput(Validation *context) const;

 private:
  Big32 monitorId_;
  PortNumber outPort_;
  GroupNumber outGroup_;
  Big<OFPFlowMonitorFlags> flags_;
  TableNumber tableId_;
  Big<OFPFlowMonitorCommand> command_;
  MatchHeader matchHeader_;
  Padding<4> pad_;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 20,
    SizeWithoutMatchHeader = 16
  };

  MPFlowMonitorRequest() = default;

  friend class MPFlowMonitorRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPFlowMonitorRequest) == 24, "Unexpected size.");
static_assert(IsStandardLayout<MPFlowMonitorRequest>(),
              "Expected standard layout.");

class MPFlowMonitorRequestBuilder {
 public:
  void setMonitorId(UInt32 monitorId) { msg_.monitorId_ = monitorId; }
  void setOutPort(PortNumber outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(GroupNumber outGroup) { msg_.outGroup_ = outGroup; }
  void setFlags(OFPFlowMonitorFlags flags) { msg_.flags_ = flags; }
  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setCommand(OFPFlowMonitorCommand command) { msg_.command_ = command; }

  void setMatch(const MatchBuilder &match) { match_ = match; }

  void write(Writable *channel);

 private:
  MPFlowMonitorRequest msg_;
  MatchBuilder match_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPFLOWMONITORREQUEST_H_
