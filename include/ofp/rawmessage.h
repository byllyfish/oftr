#ifndef OFP_RAWMESSAGE_H_
#define OFP_RAWMESSAGE_H_

#include "ofp/bytelist.h"
#include "ofp/header.h"

namespace ofp {

class Writable;

/// Concrete class for a raw, not-necessarily-legal OpenFlow message.
///
/// We only support outgoing raw message's.

class RawMessageBuilder {
 public:
  explicit RawMessageBuilder(UInt32 xid) : header_{OFPT_UNSUPPORTED} {
    header_.setXid(xid);
  }

  void setType(OFPType type) { header_.setType(type); }
  void setData(const ByteList &data) { data_ = data; }

  UInt32 send(Writable *channel);

 private:
  Header header_;
  ByteList data_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_RAWMESSAGE_H_
