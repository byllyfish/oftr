//  ===== ---- ofp/mpflowstatsrequest.h --------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines MPFlowStatsRequest and MPFlowStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPFLOWSTATSREQUEST_H_
#define OFP_MPFLOWSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/matchbuilder.h"
#include "ofp/match.h"

namespace ofp {

class Writable;
class MultipartRequest;

class MPFlowStatsRequest {
public:
  static const MPFlowStatsRequest *cast(const MultipartRequest *req);

  MPFlowStatsRequest() = default;

  UInt8 tableId() const { return tableId_; }
  UInt32 outPort() const { return outPort_; }
  UInt32 outGroup() const { return outGroup_; }
  UInt64 cookie() const { return cookie_; }
  UInt64 cookieMask() const { return cookieMask_; }

  Match match() const;

  bool validateLength(size_t length) const;

private:
  Big8 tableId_;
  Padding<3> pad_1;
  Big32 outPort_;
  Big32 outGroup_;
  Padding<4> pad_2;
  Big64 cookie_;
  Big64 cookieMask_;

  Big16 matchType_ = 0;
  Big16 matchLength_ = 0;
  Padding<4> pad_3;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 36,
    SizeWithoutMatchHeader = 32,
    HeaderSize = 4
  };

  friend class MPFlowStatsRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPFlowStatsRequest) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPFlowStatsRequest>(),
              "Expected standard layout.");

class MPFlowStatsRequestBuilder {
public:
  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setOutPort(UInt32 outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(UInt32 outGroup) { msg_.outGroup_ = outGroup; }
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
