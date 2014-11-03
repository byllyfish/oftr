#ifndef OFP_OXMIDLIST_H_
#define OFP_OXMIDLIST_H_

#include "ofp/protocollist.h"
#include "ofp/oxmidrange.h"

namespace ofp {

class OXMIDList : public ProtocolList<OXMIDRange> {
  using Inherited = ProtocolList<OXMIDRange>;

 public:
  using Inherited::Inherited;

  void add(const OXMID &id) { buf_.add(&id, id.length()); }
};

}  // namespace ofp

#endif  // OFP_OXMIDLIST_H_
