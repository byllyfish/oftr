// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_UNRECOGNIZEDPROPERTY_H_
#define OFP_UNRECOGNIZEDPROPERTY_H_

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class UnrecognizedProperty : private NonCopyable {
 public:
  UnrecognizedProperty(UInt16 type, ByteRange data)
      : type_{type},
        length_{UInt16_narrow_cast(data.size() + FixedHeaderSize)},
        range_{data} {}

  size_t size() const { return length_; }

  using ValueType = ByteRange;
  ValueType value() const {
    return SafeByteRange(this, length_, FixedHeaderSize);
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
