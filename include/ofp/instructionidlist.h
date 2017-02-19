// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_INSTRUCTIONIDLIST_H_
#define OFP_INSTRUCTIONIDLIST_H_

#include "ofp/instructionidrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class InstructionIDList : public ProtocolList<InstructionIDRange> {
  using Inherited = ProtocolList<InstructionIDRange>;

 public:
  using Inherited::Inherited;

  void add(const InstructionID &id) { buf_.add(&id, id.length()); }
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONIDLIST_H_
