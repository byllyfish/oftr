#ifndef OFP_VALIDATION_H_
#define OFP_VALIDATION_H_

#include "ofp/byteorder.h"
#include "ofp/byterange.h"
#include "ofp/constants.h"

namespace ofp {

class Validation {
public:
    explicit Validation(const UInt8 *data, size_t length) : data_{data}, length_{length} {}
    explicit Validation(const ByteRange &range) : Validation{range.data(), range.size()} {}

    const UInt8 *data() const { return data_; }
    size_t length() const { return length_; }

    size_t lengthRemaining() const { return lengthRemaining_; }
    void setLengthRemaining(size_t length) { lengthRemaining_ = length; }

    // Log validation failure messages.
    
    void messageSizeIsInvalid();
    void lengthRemainingIsInvalid(const UInt8 *ptr, size_t expectedLength);
    
    void rangeSizeHasImproperAlignment(const UInt8 *ptr, size_t alignment);
    void rangeDataHasImproperAlignment(const UInt8 *ptr, size_t alignment);
    void rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize);
    void rangeElementSizeHasImproperAlignment(const UInt8 *ptr, size_t elemSize, size_t alignment);
    void rangeElementSizeOverrunsEnd(const UInt8 *ptr, size_t jumpSize);
    void rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr, size_t elementSize);

    // Used in validating multipart section lengths.
    
    bool validateLength(size_t length, size_t minSize) {
      return (length >= minSize) && ((length % 8) == 0);
    }

    // Used in validating multipart request/reply.
    
    bool validateEmpty(const UInt8 *body);

    template <class Type>
    bool validate(const UInt8 *body);

    template <class Type, size_t Offset = Type::MPVariableSizeOffset>
    bool validateArrayVariableSize(const UInt8 *body);

    template <class Type>
    bool validateArrayFixedSize(const UInt8 *body);

private:
    const UInt8 *data_;
    const size_t length_;
    size_t lengthRemaining_ = 0;

    size_t offset(const UInt8 *ptr) const;
    std::string hexContext(const UInt8 *ptr) const;
    void logContext(const UInt8 *ptr) const;
};


inline bool Validation::validateEmpty(const UInt8 *body) {
  if (lengthRemaining() != 0) {
    lengthRemainingIsInvalid(body, 0);
    return false;
  }

  return true;
}

template <class Type>
bool Validation::validate(const UInt8 *body) {
  const Type *ptr = reinterpret_cast<const Type *>(body);
  // Class is responsible for validating length.
  return ptr->validateInput(this);
}

// There are two versions of validateArray; variable size and fixed size.

template <class Type, size_t Offset>
bool Validation::validateArrayVariableSize(const UInt8 *body) {
  size_t length = lengthRemaining();

  if ((length % 8) != 0) {
    rangeSizeHasImproperAlignment(body, 8);
    return false;
  }

  if (!IsPtrAligned(body, 8)) {
    rangeDataHasImproperAlignment(body, 8);
    return false;
  }

  while (length != 0) {
    assert(IsPtrAligned(body, 8) && "Misaligned ptr");

    if (length < Offset + sizeof(Big16)) {
        rangeElementSizeIsTooSmall(body, Offset + sizeof(Big16));
        return false;
    }

    size_t size = *Big16_cast(body + Offset);
    if (size > length) {
        rangeElementSizeOverrunsEnd(body, size);
        return false;
    }

    if ((size % 8) != 0) {
        rangeElementSizeHasImproperAlignment(body, size, 8);
        return false;
    }

    const Type *ptr = reinterpret_cast<const Type *>(body);
    if (!ptr->validateInput(this)) {
        return false;
    }

    body += size;
    length -= size;
  }

  return true;
}

template <class Type>
bool Validation::validateArrayFixedSize(const UInt8 *body) {
  size_t length = lengthRemaining();

  if (!IsPtrAligned(body, 8)) {
    rangeDataHasImproperAlignment(body, 8);
    return false;
  }

  const size_t size = sizeof(Type);
  static_assert(size >= 8, "Expected size >= 8");
  static_assert((size % 8) == 0, "Expected size as multiple of 8.");

  if ((length % size) != 0) {
    rangeSizeIsNotMultipleOfElementSize(body, size);
    return false;
  }

  while (length != 0) {
    const Type *ptr = reinterpret_cast<const Type *>(body);
    assert(IsPtrAligned(ptr, 8) && "Misaligned ptr");

    if (size > length) {
        rangeElementSizeOverrunsEnd(body, size);
        return false;
    }

    if (!ptr->validateInput(this)) {
        return false;
    }

    body += size;
    length -= size;
  }

  return true;
}

}  // namespace ofp

#endif // OFP_VALIDATION_H_
