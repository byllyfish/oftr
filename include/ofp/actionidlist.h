#ifndef OFP_ACTIONIDLIST_H_
#define OFP_ACTIONIDLIST_H_

#include "ofp/actionidrange.h"
#include "ofp/protocollist.h"

namespace ofp {

using ActionIDList = ProtocolList<ActionIDRange>;


}  // namespace ofp

#endif // OFP_ACTIONIDLIST_H_
