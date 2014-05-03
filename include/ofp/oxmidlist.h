#ifndef OFP_OXMIDLIST_H_
#define OFP_OXMIDLIST_H_

#include "ofp/protocollist.h"
#include "ofp/oxmidrange.h"

namespace ofp {

using OXMIDList = ProtocolList<OXMIDRange>;

}  // namespace ofp

#endif // OFP_OXMIDLIST_H_
