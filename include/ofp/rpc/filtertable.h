#ifndef OFP_RPC_TABLEFILTER_H_
#define OFP_RPC_TABLEFILTER_H_

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

#endif  // OFP_RPC_TABLEFILTER_H_
