#ifndef OFP_RPC_RPCFILTERACTION_H_
#define OFP_RPC_RPCFILTERACTION_H_

namespace ofp {
namespace rpc {

class RpcFilterAction {
public:
    virtual ~RpcFilterAction() { }

    virtual bool apply(ByteRange enetFrame, Message *message, bool *escalate) = 0;
};

}  // namespace rpc
}  // namespace ofp

#endif // OFP_RPC_RPCFILTERACTION_H_
