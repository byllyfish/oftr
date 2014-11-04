// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPPORTSTATSREQUEST_H_
#define OFP_MPPORTSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPPortStatsRequest {
 public:
  static const MPPortStatsRequest *cast(const MultipartRequest *req);

  MPPortStatsRequest() = default;

  UInt32 portNo() const { return portNo_; }

  bool validateInput(Validation *context) const;

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
