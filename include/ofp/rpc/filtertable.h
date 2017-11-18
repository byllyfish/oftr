// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_FILTERTABLE_H_
#define OFP_RPC_FILTERTABLE_H_

#include <vector>
#include "ofp/rpc/filtertableentry.h"

namespace ofp {

class Message;

namespace rpc {

class RpcServer;

class FilterTable {
 public:
  bool apply(Message *message, bool *escalate);

 private:
  std::vector<FilterTableEntry> table_;
};

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_FILTERTABLE_H_
