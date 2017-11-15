#include "ofp/rpc/rpcfiltertableentry.h"
#include "ofp/rpc/rpcfilteraction.h"
#include "ofp/message.h"

using namespace ofp::rpc;


bool RpcFilterTableEntry::apply(ByteRange data, Message *message, bool *escalate) {
    assert(message->type() == OFPT_PACKET_IN);

    if (!pktFilter_.match(data)) {
        log_debug("RpcFilterTableEntry::apply - data doesn't match filter");
        return false;
    }

    if (action_) {
        if (!action_->apply(data, message, escalate)) {
            log_debug("RpcFilterTableEntry::apply - action doesn't match");
            return false;
        }
    }

    return true;
}
