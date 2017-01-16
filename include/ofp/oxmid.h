// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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

  bool validateInput(Validation *context) const { return true; }

 private:
  OXMType type_;
  Big32 experimenter_;

  size_t length() const { return type_.isExperimenter() ? 8 : 4; }

  friend class OXMIDList;
};

}  // namespace ofp

#endif  // OFP_OXMID_H_
