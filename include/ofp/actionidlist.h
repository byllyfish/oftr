#ifndef OFP_ACTIONIDLIST_H_
#define OFP_ACTIONIDLIST_H_

#include "ofp/actionidrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class ActionIDList : public ProtocolList<ActionIDRange> {
using Inherited = ProtocolList<ActionIDRange>;
public:
    using Inherited::Inherited;

    void add(const ActionID &id) {
        buf_.add(&id, id.length());
    }
};

}  // namespace ofp

#endif // OFP_ACTIONIDLIST_H_
