#ifndef OFP_RPC_RPCTABLEFILTER_H_
#define OFP_RPC_RPCTABLEFILTER_H_

#include <vector>
#include "ofp/rpc/rpcfiltertableentry.h"

namespace ofp {

class Message;

namespace rpc {

class RpcServer;

class RpcFilterTable {
public:
    bool apply(Message *message, bool *escalate);

private:
    std::vector<RpcFilterTableEntry> table_;
};

}  // namespace rpc
}  // namespace ofp

#endif // OFP_RPC_RPCTABLEFILTER_H_
