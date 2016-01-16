// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPQUEUESTATSREQUEST_H_
#define OFP_MPQUEUESTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/queuenumber.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPQueueStatsRequest {
 public:
  static const MPQueueStatsRequest *cast(const MultipartRequest *req);

  MPQueueStatsRequest() = default;

  PortNumber portNo() const { return portNo_; }
  QueueNumber queueId() const { return queueId_; }

  bool validateInput(Validation *context) const;

 private:
  PortNumber portNo_;
  QueueNumber queueId_;

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

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setQueueId(QueueNumber queueId) { msg_.queueId_ = queueId; }

  void write(Writable *channel);

 private:
  MPQueueStatsRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPQUEUESTATSREQUEST_H_
