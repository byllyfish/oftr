// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_OXMID_H_
#define OFP_OXMID_H_

#include "ofp/oxmtype.h"

namespace ofp {

class OXMID {
 public:
  enum {
    ProtocolIteratorSizeOffset = PROTOCOL_ITERATOR_SIZE_CONDITIONAL,
    ProtocolIteratorAlignment = 4
  };

  explicit OXMID(OXMType type = {}, UInt32 experimenter = 0)
      : type_{type}, experimenter_{experimenter} {}

  OXMType type() const { return type_; }
  UInt32 experimenter() const { return experimenter_; }

 private:
  OXMType type_;
  Big32 experimenter_;

  size_t length() const { return type_.isExperimenter() ? 8 : 4; }

  friend class OXMIDList;
};

}  // namespace ofp

#endif  // OFP_OXMID_H_
