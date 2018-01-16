// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
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
  FilterTable() = default;

  bool apply(Message *message, bool *escalate);

  size_t size() const { return table_.size(); }

  void setFilters(std::vector<FilterTableEntry> &&filters) {
    table_ = std::move(filters);
  }

 private:
  std::vector<FilterTableEntry> table_;

  FilterTable(const FilterTable &) = delete;
  FilterTable &operator=(const FilterTable &) = delete;
};

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_FILTERTABLE_H_
