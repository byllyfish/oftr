// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMIDLIST_H_
#define OFP_OXMIDLIST_H_

#include "ofp/oxmidrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class OXMIDList : public ProtocolList<OXMIDRange> {
  using Inherited = ProtocolList<OXMIDRange>;

 public:
  using Inherited::Inherited;

  void add(const OXMID &id) { buf_.add(&id, id.length()); }
};

}  // namespace ofp

#endif  // OFP_OXMIDLIST_H_
