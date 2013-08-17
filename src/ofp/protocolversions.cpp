//  ===== ---- ofp/protocolversions.cpp --------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements a concrete class that represents a set of OpenFlow
/// protocol versions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>

ProtocolVersions::ProtocolVersions(std::initializer_list<UInt8> versions)
    : bitmap_{0}
{
    for (auto v : versions) {
        assert(v <= MaxVersion);
        if (v <= MaxVersion)
            bitmap_ |= (1 << v);
    }
}

UInt8 ProtocolVersions::highestVersion() const
{
    UInt32 bits = bitmap_;
    for (unsigned i = MaxVersion; i > 0; --i) {
        if ((bits >> i) & 1) {
            return UInt8_narrow_cast(i);
        }
    }
    return 0;
}

ProtocolVersions ofp::ProtocolVersions::fromBitmap(UInt32 bitmap)
{
    ProtocolVersions versions;
    versions.bitmap_ = bitmap;
    return versions;
}

} // </namespace ofp>
