//  ===== ---- ofp/protocolversions.cpp --------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
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
        if (v == 0)
            bitmap_ = All;
        else if (v <= MaxVersion)
            bitmap_ |= (1 << v);
    }
}

ProtocolVersions::ProtocolVersions(const std::vector<UInt8> &versions)
    : bitmap_{0}
{
    for (auto v : versions) {
        if (v == 0)
            bitmap_ = All;
        else if (v <= MaxVersion)
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

std::vector<UInt8> ProtocolVersions::versions() const
{
    std::vector<UInt8> result;
    UInt32 bits = bitmap_;
    for (UInt8 i = 1; i <= MaxVersion; ++i) {
        if ((bits >> i) & 1)
            result.push_back(i);
    }
    return result;
}

} // </namespace ofp>
