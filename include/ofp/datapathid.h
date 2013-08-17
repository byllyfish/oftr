#ifndef OFP_DATAPATHID_H
#define OFP_DATAPATHID_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class DatapathID {
public:
    enum {
        Length = 8
    };

    using ArrayType = std::array<UInt8, Length>;

    DatapathID() = default;

    DatapathID(const ArrayType dpid)
    {
    	dpid_ = dpid;
    }

    std::string toString() const;

private:
    ArrayType dpid_;
};

} // </namespace ofp>

#endif // OFP_DATAPATHID_H
