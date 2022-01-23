// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmregister.h"

#include <sstream>

#include "ofp/log.h"

using namespace ofp;

bool OXMRegister::valid() const {
  return (nbits_ > 0) && (offset_ + nbits_ <= maxBits());
}

bool OXMRegister::parse(llvm::StringRef s) {
  llvm::StringRef reg = s.trim();

  size_t pos = reg.find('[');
  if (pos == llvm::StringRef::npos) {
    // Treat the entire string as a field name.
    if (!type_.parse(reg)) {
      log_warning("OXMRegister: Unknown field name", reg);
      return false;
    }
    offset_ = 0;
    nbits_ = maxBits();
    return true;
  }

  llvm::StringRef field = reg.substr(0, pos);
  llvm::StringRef bits = reg.substr(pos + 1);

  // Parse field name.
  if (!type_.parse(field)) {
    log_warning("OXMRegister: Unknown field name", reg);
    return false;
  }

  if (bits.empty() || bits.back() != ']') {
    log_warning("OXMRegister: Invalid format", reg);
    return false;
  }

  bits = bits.drop_back();  // drop ']'

  // Split `offset` and `offsetEnd` on ':'

  auto offsets = bits.split(':');
  offsets.first = offsets.first.trim();
  offsets.second = offsets.second.trim();

  int offset = 0;
  int offsetEnd = 0;

  if (offsets.first.empty()) {
    offset = 0;
  } else if (offsets.first.getAsInteger(10, offset)) {
    log_warning("OXMRegister: Invalid format", reg);
    return false;
  }

  if (offsets.second.empty()) {
    // Make sure the colon was present.
    if (bits.find(':') == llvm::StringRef::npos) {
      log_warning("OXMRegister: Invalid format", reg);
      return false;
    }
    offsetEnd = maxBits();
  } else if (offsets.second.getAsInteger(10, offsetEnd)) {
    log_warning("OXMRegister: Invalid format", reg);
    return false;
  }

  int nbits = offsetEnd - offset;
  if (offset < 0 || nbits <= 0 || offset + nbits > maxBits()) {
    log_warning("OXMRegister: Invalid bits specified", reg, offset, nbits,
                maxBits());
    return false;
  }

  nbits_ = UInt16_narrow_cast(nbits);
  offset_ = UInt16_narrow_cast(offset);

  return true;
}
