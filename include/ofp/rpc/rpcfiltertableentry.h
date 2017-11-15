#ifndef OFP_RPC_RPCFILTERTABLEENTRY_H_
#define OFP_RPC_RPCFILTERTABLEENTRY_H_

#include "ofp/demux/pktfilter.h"

namespace ofp {

class Message;

namespace rpc {

class RpcServer;
class RpcFilterAction;

OFP_BEGIN_IGNORE_PADDING

/// \brief A concrete class for a filter table entry.

class RpcFilterTableEntry {
public:
    bool setFilter(const std::string &filter);

    bool apply(ByteRange data, Message *message, bool *escalate);

private:
    demux::PktFilter pktFilter_;
    RpcFilterAction *action_ = nullptr;
    bool escalate_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif // OFP_RPC_RPCFILTERTABLEENTRY_H_
