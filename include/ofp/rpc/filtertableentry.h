// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_FILTERTABLEENTRY_H_
#define OFP_RPC_FILTERTABLEENTRY_H_

#include "ofp/demux/pktfilter.h"
#include "ofp/portnumber.h"
#include "ofp/rpc/filteraction.h"
#include "ofp/rpc/ratelimiter.h"

namespace ofp {

class Message;

namespace rpc {

class RpcServer;

OFP_BEGIN_IGNORE_PADDING

/// \brief A concrete class for a filter table entry.

class FilterTableEntry {
 public:
  FilterTableEntry() : escalate_{false} {}

  bool setFilter(const std::string &filter) {
    return pktFilter_.setFilter(filter);
  }

  void setAction(std::unique_ptr<FilterAction> &&action) {
    action_ = std::move(action);
  }

  void setEscalate(const RateLimiter &limiter) { escalate_ = limiter; }

  bool apply(ByteRange data, PortNumber inPort, UInt64 metadata,
             Message *message, bool *escalate);

 private:
  demux::PktFilter pktFilter_;
  std::unique_ptr<FilterAction> action_;
  RateLimiter escalate_;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_FILTERTABLEENTRY_H_
