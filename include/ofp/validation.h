// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_VALIDATION_H_
#define OFP_VALIDATION_H_

#include "ofp/byteorder.h"
#include "ofp/byterange.h"
#include "ofp/constants.h"
#include "ofp/padding.h"

namespace ofp {

class Message;

OFP_BEGIN_IGNORE_PADDING

class Validation {
 public:
  explicit Validation(const Message *msg = nullptr, OFPErrorCode *error = &defaultPtr_);

  UInt8 version() const { return version_; }
  size_t length() const { return length_; }
  size_t lengthRemaining() const { return lengthRemaining_; }
  void setLengthRemaining(size_t length) { lengthRemaining_ = length; }

  // Log validation failure messages.

  void messageSizeIsInvalid();
  void messageTypeIsNotSupported();
  void multipartTypeIsNotSupported();
  void multipartTypeIsNotSupportedForVersion();
  void multipartSizeHasImproperAlignment();
  void lengthRemainingIsInvalid(const UInt8 *ptr, size_t expectedLength);

  void rangeSizeHasImproperAlignment(const UInt8 *ptr,
                                     ProtocolIteratorType type);
  void rangeDataHasImproperAlignment(const UInt8 *ptr, size_t alignment);
  void rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize);
  void rangeElementSizeHasImproperAlignment(const UInt8 *ptr, size_t elemSize,
                                            size_t alignment);
  void rangeElementSizeOverrunsEnd(const UInt8 *ptr, size_t jumpSize);
  void rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr,
                                           size_t elementSize);

  // Used in validating multipart section lengths.

  bool validateLength(size_t length, size_t minSize) {
    return (length >= minSize) && ((length % 8) == 0);
  }

  // Used in validating multipart request/reply.

  bool validateEmpty(const UInt8 *body, UInt8 minVersion);

  template <class Type>
  bool validate(const UInt8 *body, UInt8 minVersion, bool lenMultiple8 = true);

  template <class Type, size_t Offset = Type::MPVariableSizeOffset>
  bool validateArrayVariableSize(const UInt8 *body, UInt8 minVersion);

  template <class Type>
  bool validateArrayFixedSize(const UInt8 *body, UInt8 minVersion);

 private:
  const Message *msg_;
  size_t length_;
  size_t lengthRemaining_ = 0;
  UInt8 version_;
  OFPErrorCode *error_;

  size_t offset(const UInt8 *ptr) const;
  std::string hexContext(const UInt8 *ptr) const;
  void logContext(const UInt8 *ptr = nullptr) const;

  void setErrorMessage(const char *errorMessage, size_t errorOffset = 0);

  static OFPErrorCode defaultPtr_;
};

OFP_END_IGNORE_PADDING

inline bool Validation::validateEmpty(const UInt8 *body, UInt8 minVersion) {
  if (version_ < minVersion) {
    multipartTypeIsNotSupportedForVersion();
    return false;
  }
  if (lengthRemaining() != 0) {
    lengthRemainingIsInvalid(body, 0);
    return false;
  }

  return true;
}

template <class Type>
bool Validation::validate(const UInt8 *body, UInt8 minVersion,
                          bool lenMultiple8) {
  if (version_ < minVersion) {
    multipartTypeIsNotSupportedForVersion();
    return false;
  }
  if (lenMultiple8 && (lengthRemaining() % 8) != 0) {
    multipartSizeHasImproperAlignment();
    return false;
  }
  const Type *ptr = reinterpret_cast<const Type *>(body);
  // Class is responsible for validating length.
  return ptr->validateInput(this);
}

// There are two versions of validateArray; variable size and fixed size.

template <class Type, size_t Offset>
bool Validation::validateArrayVariableSize(const UInt8 *body,
                                           UInt8 minVersion) {
  if (version_ < minVersion) {
    multipartTypeIsNotSupportedForVersion();
    return false;
  }

  size_t length = lengthRemaining();
  if ((length % 8) != 0) {
    rangeSizeHasImproperAlignment(body, ProtocolIteratorType::Unspecified);
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

    if (size < Offset + sizeof(Big16)) {
      rangeElementSizeIsTooSmall(body, Offset + sizeof(Big16));
      return false;
    }

    size = PadLength(size);
    if (size > length) {
      rangeElementSizeOverrunsEnd(body, size);
      return false;
    }

    // if ((size % 8) != 0) {
    //  rangeElementSizeHasImproperAlignment(body, size, 8);
    //  return false;
    //}

    const Type *ptr = reinterpret_cast<const Type *>(body);
    if (!ptr->validateInput(this)) {
      return false;
    }

    assert(size > 0);

    body += size;
    length -= size;
  }

  return true;
}

template <class Type>
bool Validation::validateArrayFixedSize(const UInt8 *body, UInt8 minVersion) {
  if (version_ < minVersion) {
    multipartTypeIsNotSupportedForVersion();
    return false;
  }

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

#endif  // OFP_VALIDATION_H_
