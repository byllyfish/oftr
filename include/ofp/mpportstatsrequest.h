//  ===== ---- ofp/mpportstatsrequest.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the MPPortStatsRequest and MPPortStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPPORTSTATSREQUEST_H_
#define OFP_MPPORTSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class MultipartRequest;

class MPPortStatsRequest {
 public:
  static const MPPortStatsRequest *cast(const MultipartRequest *req);

  MPPortStatsRequest() = default;

  UInt32 portNo() const { return portNo_; }

  bool validateInput(size_t length) const;
  
 private:
  Big32 portNo_;
  Padding<4> pad_;

  friend class MPPortStatsRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPPortStatsRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPPortStatsRequest>(),
              "Expected standard layout.");

class MPPortStatsRequestBuilder {
 public:
  MPPortStatsRequestBuilder() = default;

  void setPortNo(UInt32 portNo) { msg_.portNo_ = portNo; }

  void write(Writable *channel);

 private:
  MPPortStatsRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPPORTSTATSREQUEST_H_
