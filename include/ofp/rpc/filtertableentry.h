#ifndef OFP_RPC_FILTERTABLEENTRY_H_
#define OFP_RPC_FILTERTABLEENTRY_H_

#include "ofp/demux/pktfilter.h"
#include "ofp/portnumber.h"

namespace ofp {

class Message;

namespace rpc {

class RpcServer;
class FilterAction;

OFP_BEGIN_IGNORE_PADDING

/// \brief A concrete class for a filter table entry.

class FilterTableEntry {
 public:
  bool setFilter(const std::string &filter);

  bool apply(ByteRange data, PortNumber inPort, Message *message,
             bool *escalate);

 private:
  demux::PktFilter pktFilter_;
  FilterAction *action_ = nullptr;
  bool escalate_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_FILTERTABLEENTRY_H_
