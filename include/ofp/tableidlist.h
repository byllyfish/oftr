#ifndef OFP_TABLEIDLIST_H_
#define OFP_TABLEIDLIST_H_

#include "ofp/bytelist.h"
#include "ofp/tableidrange.h"

namespace ofp {

class TableIDList {
public:

    void add(UInt8 tableId) { buf_.add(&tableId, sizeof(tableId)); }

    TableIDRange toRange() const { return TableIDRange{buf_.toRange()}; }
    
private:
    ByteList buf_;
};

}  // namespace ofp

#endif // OFP_TABLEIDLIST_H_
