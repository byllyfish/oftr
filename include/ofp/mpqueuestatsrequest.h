// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPQUEUESTATSREQUEST_H_
#define OFP_MPQUEUESTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPQueueStatsRequest {
 public:
  static const MPQueueStatsRequest *cast(const MultipartRequest *req);

  MPQueueStatsRequest() = default;

  UInt32 portNo() const { return portNo_; }
  UInt32 queueId() const { return queueId_; }

  bool validateInput(Validation *context) const;

 private:
  Big32 portNo_;
  Big32 queueId_;

  friend class MPQueueStatsRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPQueueStatsRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPQueueStatsRequest>(),
              "Expected standard layout.");

class MPQueueStatsRequestBuilder {
 public:
  MPQueueStatsRequestBuilder() = default;

  void setPortNo(UInt32 portNo) { msg_.portNo_ = portNo; }
  void setQueueId(UInt32 queueId) { msg_.queueId_ = queueId; }

  void write(Writable *channel);

 private:
  MPQueueStatsRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPQUEUESTATSREQUEST_H_
