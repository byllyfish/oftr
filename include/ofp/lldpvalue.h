// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_LLDPVALUE_H_
#define OFP_LLDPVALUE_H_

namespace ofp {

class LLDPValue {
 public:
  LLDPValue() { std::memset(buf_, 0, sizeof(buf_)); }
  /* implicit NOLINT */ LLDPValue(const ByteRange &val) {
    assign(val.data(), val.size());
  }

  const UInt8 *data() const { return &buf_[1]; }
  size_t size() const { return buf_[0] <= MaxSize ? buf_[0] : MaxSize; }

  UInt8 *mutableData() { return &buf_[1]; }
  void resize(size_t size) {
    assert(size <= MaxSize);
    buf_[0] = UInt8_narrow_cast(size);
  }

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
};

}  // namespace ofp

#endif  // OFP_LLDPVALUE_H_
