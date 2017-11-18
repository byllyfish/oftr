#ifndef OFP_RPC_FILTERACTION_H_
#define OFP_RPC_FILTERACTION_H_

#include "ofp/byterange.h"
#include "ofp/portnumber.h"

namespace ofp {

class Message;

namespace rpc {

class FilterAction {
public:
    virtual ~FilterAction() { }

    virtual bool apply(ByteRange enetFrame, PortNumber inPort, Message *message) = 0;
};

}  // namespace rpc
}  // namespace ofp

#endif // OFP_RPC_FILTERACTION_H_
