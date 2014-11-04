// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_UNRECOGNIZEDPROPERTY_H_
#define OFP_UNRECOGNIZEDPROPERTY_H_

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class UnrecognizedProperty {
 public:
  UnrecognizedProperty(UInt16 type, ByteRange data)
      : type_{type},
        length_{UInt16_narrow_cast(data.size() + FixedHeaderSize)},
        range_{data} {}

  size_t size() const { return length_; }

  using ValueType = ByteRange;
  ValueType value() const {
    return ByteRange{BytePtr(this) + FixedHeaderSize,
                     length_ - FixedHeaderSize};
  }
  static ValueType defaultValue() { return {}; }

  enum : size_t { FixedHeaderSize = 4U };

  ByteRange valueRef() const { return range_; }

  bool validateInput() const { return true; }

 private:
  Big16 type_;
  Big16 length_;
  ByteRange range_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_UNRECOGNIZEDPROPERTY_H_
