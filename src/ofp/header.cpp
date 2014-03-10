//  ===== ---- ofp/header.cpp ------------------------------*- C++ -*- =====  //
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
/// \brief Implements Header class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/header.h"
#include "ofp/log.h"

using namespace ofp;

static UInt8 MaxTypeByVersion[] = {0,
                                   UInt8_cast(deprecated::v1::OFPT_LAST),
                                   UInt8_cast(deprecated::v2::OFPT_LAST),
                                   UInt8_cast(deprecated::v3::OFPT_LAST),
                                   UInt8_cast(OFPT_LAST)};

static OFPType translateTypeToVersion(UInt8 type, UInt8 version) {
  assert(type > OFPT_FLOW_MOD);
  assert(version <= OFP_VERSION_3);

  if (version == OFP_VERSION_1) {
    if (type == OFPT_GROUP_MOD || type == OFPT_TABLE_MOD)
      return OFPT_UNSUPPORTED;
    type = (type == OFPT_PORT_MOD) ? type - 1 : type - 2;
  }

  if (type > MaxTypeByVersion[version]) {
    return OFPT_UNSUPPORTED;
  }

  return OFPType(type);
}

static OFPType translateTypeFromVersion(UInt8 type, UInt8 version) {
  assert(type > OFPT_FLOW_MOD);
  assert(version <= OFP_VERSION_3);

  if (type > MaxTypeByVersion[version]) {
    return OFPT_UNSUPPORTED;
  }

  if (version == OFP_VERSION_1) {
    if (type == UInt8_cast(deprecated::v1::OFPT_PORT_MOD)) {
      type = OFPT_PORT_MOD;
    } else if (type > UInt8_cast(deprecated::v1::OFPT_PORT_MOD)) {
      type += 2;
    }
  }

  return OFPType(type);
}

OFPType Header::translateType(UInt8 version, UInt8 type, UInt8 newVersion) {
  assert(version >= OFP_VERSION_1);

  if (version == newVersion)
    return OFPType(type);

  assert(version == OFP_VERSION_4 || newVersion == OFP_VERSION_4);

  if (version > OFP_VERSION_4 || newVersion > OFP_VERSION_4)
    return OFPType(type);

  if (type <= OFPT_FLOW_MOD)
    return OFPType(type);

  if (version == OFP_VERSION_4) {
    return translateTypeToVersion(type, newVersion);

  } else if (newVersion == OFP_VERSION_4) {
    return translateTypeFromVersion(type, version);

  } else {
    return OFPT_UNSUPPORTED;
  }
}


bool Header::validateInput(UInt8 negotiatedVersion) const
{
  // Check length field of the header. Since length includes header, it can't
  // be smaller than 8 bytes.

  if (length_ < sizeof(Header)) {
    log::debug("Header length < 8");
    return false;
  }

  // Header version must be non-zero and match the negotiated version if there 
  // is one.

  if (!version_ || (negotiatedVersion != 0 && version_ != negotiatedVersion)) {
    log::debug("Wrong version");
    return false;
  }

  // N.B. The type field will be checked by transmogrify.

  return true;
}
