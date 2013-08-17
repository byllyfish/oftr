//  ===== ---- ofp/protocolversions.h ----------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines a concrete class that represents a set of OpenFlow
/// protocol versions.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PROTOCOL_VERSIONS_H
#define OFP_PROTOCOL_VERSIONS_H

#include "ofp/types.h"
#include <initializer_list>

namespace ofp { // <namespace ofp>

/// \brief Represents a set of OpenFloe protocol versions.
class ProtocolVersions {
public:

    enum {
        MaxVersion = 0x04
    };

    enum Setting : UInt32{All = ~(~0U << (MaxVersion + 1)) & ~1U, None = 0U};

    ProtocolVersions() : bitmap_{All}
    {
    }

    ProtocolVersions(Setting setting) : bitmap_{setting}
    {
    }

    ProtocolVersions(std::initializer_list<UInt8> versions);

    bool empty() const
    {
        return (bitmap_ == 0);
    }

    UInt8 highestVersion() const;

    UInt32 bitmap() const
    {
        return bitmap_;
    }

    ProtocolVersions intersection(ProtocolVersions versions) const
    {
        return fromBitmap(bitmap_ & versions.bitmap_);
    }

    static ProtocolVersions fromBitmap(UInt32 bitmap);

private:
    UInt32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_PROTOCOL_VERSIONS_H
