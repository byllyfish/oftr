//  ===== ---- ofp/flowstatsrequest.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines FlowStatsRequest and FlowStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FLOWSTATSREQUEST_H
#define OFP_FLOWSTATSREQUEST_H

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/matchbuilder.h"
#include "ofp/match.h"

namespace ofp { // <namespace ofp>

class Writable;
class FlowStatsRequestBuilder;
class MultipartRequest;

class FlowStatsRequest {
public:
  static const FlowStatsRequest *cast(const MultipartRequest *req);

  FlowStatsRequest() = default;

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

  enum {
    UnpaddedSizeWithMatchHeader = 36
  };
  enum {
    SizeWithoutMatchHeader = 32
  };

  friend class FlowStatsRequestBuilder;
  friend struct llvm::yaml::MappingTraits<FlowStatsRequestBuilder>;
};

static_assert(sizeof(FlowStatsRequest) == 40, "Unexpected size.");
static_assert(IsStandardLayout<FlowStatsRequest>(),
              "Expected standard layout.");

class FlowStatsRequestBuilder {
public:
  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setOutPort(UInt32 outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(UInt32 outGroup) { msg_.outGroup_ = outGroup; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setCookieMask(UInt64 cookieMask) { msg_.cookieMask_ = cookieMask; }

  void write(Writable *channel);

private:
  FlowStatsRequest msg_;
  MatchBuilder match_;

  void writeV1(Writable *channel);
  
  friend struct llvm::yaml::MappingTraits<FlowStatsRequestBuilder>;
};

} // </namespace ofp>

#endif // OFP_FLOWSTATSREQUEST_H
