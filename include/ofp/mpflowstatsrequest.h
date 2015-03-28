// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPFLOWSTATSREQUEST_H_
#define OFP_MPFLOWSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/matchbuilder.h"
#include "ofp/match.h"
#include "ofp/groupnumber.h"
#include "ofp/tablenumber.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPFlowStatsRequest {
 public:
  static const MPFlowStatsRequest *cast(const MultipartRequest *req);

  TableNumber tableId() const { return tableId_; }
  PortNumber outPort() const { return outPort_; }
  GroupNumber outGroup() const { return outGroup_; }
  UInt64 cookie() const { return cookie_; }
  UInt64 cookieMask() const { return cookieMask_; }

  Match match() const;

  bool validateInput(Validation *context) const;

 private:
  TableNumber tableId_;
  Padding<3> pad_1;
  PortNumber outPort_;
  GroupNumber outGroup_;
  Padding<4> pad_2;
  Big64 cookie_;
  Big64 cookieMask_;

  MatchHeader matchHeader_;
  Padding<4> pad_3;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 36,
    SizeWithoutMatchHeader = 32
  };

  MPFlowStatsRequest() = default;

  friend class MPFlowStatsRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPFlowStatsRequest) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPFlowStatsRequest>(),
              "Expected standard layout.");

class MPFlowStatsRequestBuilder {
 public:
  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setOutPort(PortNumber outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(GroupNumber outGroup) { msg_.outGroup_ = outGroup; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setCookieMask(UInt64 cookieMask) { msg_.cookieMask_ = cookieMask; }

  void write(Writable *channel);

 private:
  MPFlowStatsRequest msg_;
  MatchBuilder match_;

  void writeV1(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPFLOWSTATSREQUEST_H_
