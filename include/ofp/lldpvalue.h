// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LLDPVALUE_H_
#define OFP_LLDPVALUE_H_

#include "ofp/bytelist.h"

namespace ofp {

/// LLDP Value Types (separate from protocol numbers).
enum class LLDPType {
  ChassisID = 0,
  PortID = 1,
  ByteString = 2,
  OrgSpecific = 3
};

namespace detail {

bool LLDPParse(LLDPType type, const std::string &val, ByteList *data);
std::string LLDPToString(LLDPType type, const ByteRange &data);

}  // namespace detail

/// LLDPValue is a concrete type that stores an LLDP value as a
/// pascal string. The first byte is the size of the data, and
/// the remaining bytes are the data. `Type` indicates how the
/// LLDP value is parsed to/from a string.
template <LLDPType Type>
class LLDPValue {
 public:
  static LLDPType type() { return Type; }

  LLDPValue() { std::memset(buf_, 0, sizeof(buf_)); }

  /* implicit NOLINT */ LLDPValue(const ByteRange &val) {
    assign(val.data(), val.size());
  }

  const UInt8 *data() const { return &buf_[1]; }
  size_t size() const { return buf_[0] <= MaxSize ? buf_[0] : MaxSize; }
  UInt8 *mutableData() { return &buf_[1]; }

  bool empty() const { return buf_[0] == 0; }

  void resize(size_t size) {
    assert(size <= MaxSize);
    buf_[0] = UInt8_narrow_cast(size);
  }

  bool parse(const std::string &val) {
    ByteList buf;
    if (!detail::LLDPParse(Type, val, &buf))
      return false;
    assign(buf.data(), buf.size());
    return true;
  }

  ByteRange toRange() const { return ByteRange{data(), size()}; }
  std::string toString() const { return detail::LLDPToString(Type, toRange()); }

  constexpr static size_t maxSize() { return MaxSize; }

 private:
  static const size_t MaxSize = 63;

  UInt8 buf_[MaxSize + 1] = {0};

  void assign(const UInt8 *data, size_t size) {
    size_t len = size > MaxSize ? MaxSize : size;
    std::memset(buf_, 0, sizeof(buf_));
    std::memcpy(mutableData(), data, len);
    resize(len);
  }

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const LLDPValue &value) {
    return os << value.toString();
  }
};

}  // namespace ofp

#endif  // OFP_LLDPVALUE_H_
