// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMREGISTER_H_
#define OFP_OXMREGISTER_H_

#include "ofp/oxmtype.h"

namespace ofp {

/// Concrete class representing the bits of an OXM register.
///
/// A register's string representation is:
///
///     FIELD[begin:end]
///
/// `begin` is the starting bit offset (first bit is zero). `end` is the ending
/// bit offset (not inclusive).
///
/// For example, to specify the the OUI in a MacAddress:
///
///    ETH_DST[0:24]

class OXMRegister {
 public:
  OXMRegister() = default;

  explicit OXMRegister(OXMType type)
      : type_{type}, offset_{0}, nbits_{maxBits()} {}
  explicit OXMRegister(OXMType type, UInt16 offset, UInt16 nbits)
      : type_{type}, offset_{offset}, nbits_{nbits} {}

  explicit OXMRegister(const std::string &s) { (void)parse(s); }

  OXMType type() const { return type_; }
  UInt16 nbits() const { return nbits_; }
  UInt16 offset() const { return offset_; }
  UInt32 end() const { return offset_ + nbits_; }

  bool valid() const;

  bool parse(const std::string &s);
  std::string toString() const;

 private:
  OXMType type_;
  UInt16 offset_ = 0;
  UInt16 nbits_ = 0;

  UInt16 maxBits() const { return type_.oxmSize() * 8; }

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const OXMRegister &value) {
    return os << value.toString();
  }
};

static_assert(sizeof(OXMRegister) == 8, "Unexpected size.");
static_assert(alignof(OXMRegister) == 4, "Unexpected alignment.");

}  // namespace ofp

#endif  // OFP_OXMREGISTER_H_
