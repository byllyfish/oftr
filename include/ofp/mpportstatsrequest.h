// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPPORTSTATSREQUEST_H_
#define OFP_MPPORTSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPPortStatsRequest {
 public:
  static const MPPortStatsRequest *cast(const MultipartRequest *req);

  MPPortStatsRequest() = default;

  PortNumber portNo() const { return portNo_; }

  bool validateInput(Validation *context) const;

 private:
  PortNumber portNo_;
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

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }

  void write(Writable *channel);

 private:
  MPPortStatsRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPPORTSTATSREQUEST_H_
