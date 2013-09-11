//  ===== ---- ofp/datapathid.h ----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the DatapathID class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DATAPATHID_H
#define OFP_DATAPATHID_H

#include "ofp/types.h"
#include "ofp/enetaddress.h"
#include "ofp/byteorder.h"
#include <array>

namespace ofp { // <namespace ofp>

class DatapathID {
public:
    enum {
        Length = 8
    };

    using ArrayType = std::array<UInt8, Length>;

    DatapathID() : dpid_{} {}

    DatapathID(const ArrayType dpid)
    {
        dpid_ = dpid;
    }

    DatapathID(Big16 implementerDefined, EnetAddress macAddress);

    Big16 implementerDefined() const;
    EnetAddress macAddress() const;
    std::string toString() const;

    bool parse(const std::string &s);
    
    bool operator<(const DatapathID &rhs) const {
        return dpid_ < rhs.dpid_;
    }

    bool operator>(const DatapathID &rhs) const {
        return dpid_ > rhs.dpid_;
    }

    bool operator==(const DatapathID &rhs) const {
        return dpid_ == rhs.dpid_;
    }

    bool operator!=(const DatapathID &rhs) const {
        return !operator==(rhs);
    }

private:
    ArrayType dpid_;
};

std::ostream &operator<<(std::ostream &os, const DatapathID &value);

inline std::ostream &operator<<(std::ostream &os, const DatapathID &value)
{
    return os << value.toString();
}

} // </namespace ofp>

#endif // OFP_DATAPATHID_H
