// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/vlannumber.h"

#include "ofp/constants.h"
#include "ofp/log.h"

using namespace ofp;

// Translate VlanNumber from "display" format to internal value which uses
// the vlan present flag.
//
// Internal             Display           Notes
// -------------        -----------       ----------------------
//     0                0                 Tag not present
//     1-4095           -n                Invalid range
//     4096             4096              Tag present, vlan = 0, Infrequent use
//     4097-8191        1-4095            Tag present, vlan = n
//     >= 8192          -n                Invalid range

SignedInt32 VlanNumber::displayCode() const {
  SignedInt32 value = vlan_;

  if (value == 0 || value == OFPVID_PRESENT) {
    return value;
  }

  if (value < OFPVID_PRESENT || value >= 2 * OFPVID_PRESENT) {
    return -value;
  }

  return value - OFPVID_PRESENT;
}

void VlanNumber::setDisplayCode(SignedInt32 code) {
  if (code == 0 || code == OFPVID_PRESENT) {
    vlan_ = UInt16_narrow_cast(code);

  } else if (code < 0) {
    vlan_ = (-code) & 0xffff;

  } else if (code < OFPVID_PRESENT) {
    vlan_ = UInt16_narrow_cast(code + OFPVID_PRESENT);

  } else {
    log_error("Unexpected vlan display code:", code);
    vlan_ = (code & OFPVID_MASK) + OFPVID_PRESENT;
  }
}
