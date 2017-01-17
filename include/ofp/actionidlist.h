// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ACTIONIDLIST_H_
#define OFP_ACTIONIDLIST_H_

#include "ofp/actionidrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class ActionIDList : public ProtocolList<ActionIDRange> {
  using Inherited = ProtocolList<ActionIDRange>;

 public:
  using Inherited::Inherited;

  void add(const ActionID &id) { buf_.add(&id, id.length()); }
};

}  // namespace ofp

#endif  // OFP_ACTIONIDLIST_H_
