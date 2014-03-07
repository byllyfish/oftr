#include "ofp/protocoliterator.h"
#include "ofp/log.h"
#include <algorithm>

bool ofp::detail::IsProtocolIteratorValid(const ByteRange &range,
                                          const char *context,
                                          size_t minElemSize) {
  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  // Length must be a multiple of 8.
  if ((len % 8) != 0) {
    log::info("Array length is not a multiple of 8:", context);
    return false;
  }

  // Beginning pointer must be 8-byte aligned.
  if (!IsPtrAligned<8>(ptr)) {
    log::info("Array start is not 8-byte aligned:", context);
    return false;
  }

  // All of the element lengths must add up (when padded).
  while (len > 0) {
    assert(len >= 8);

    UInt16 elemSize = *Big16_cast(ptr + 2);
    if (elemSize < minElemSize) {
      log::info("Array element size less than minimum:", context);
      return false;
    }

    size_t jumpSize = PadLength(elemSize);
    if (jumpSize > len) {
      log::info("Array element size overruns the end:", context);
      return false;
    }

    len -= jumpSize;
    ptr += jumpSize;
  }

  assert(len == 0);
  assert(ptr == range.end());

  return true;
}

size_t ofp::detail::ProtocolIteratorItemCount(const ByteRange &range) {
  size_t count = 0;

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  assert((len % 8) == 0);
  assert(IsPtrAligned<8>(ptr));

  while (len > 0) {
    assert(len >= 8);

    size_t elemSize = std::max<size_t>(4, *Big16_cast(ptr + 2));
    assert(elemSize > 0);

    size_t jumpSize = PadLength(elemSize);
    if (jumpSize > len) break;

    len -= jumpSize;
    ptr += jumpSize;
    ++count;
  }

  return count;
}

/// Return true if given ByteRange is a valid protocol iterable.
///
/// To be structurally valid as a ProtocolIterable, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of 8.
///   - no element length is less than 2.
///   - the sum of each element length (padded out to 8-bytes) evenly matches
///     the size.
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.

bool ofp::detail::IsProtocolIteratorFixedTypeValid(const ByteRange &range,
                                      const char *context,
                                      size_t minElemSize) {
  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  // Length must be a multiple of 8.
  if ((len % 8) != 0) {
    log::info("Array length is not a multiple of 8:", context);
    return false;
  }

  // Beginning pointer must be 8-byte aligned.
  if (!IsPtrAligned<8>(ptr)) {
    log::info("Array start is not 8-byte aligned:", context);
    return false;
  }

  // All of the element lengths must add up (when padded).
  while (len > 0) {
    assert(len >= 8);

    UInt16 elemSize = *Big16_cast(ptr);
    if (elemSize < minElemSize) {
      log::info("Array element size less than minimum:", context);
      return false;
    }

    size_t jumpSize = PadLength(elemSize);
    if (jumpSize > len) {
      log::info("Array element size overruns the end:", context);
      return false;
    }

    len -= jumpSize;
    ptr += jumpSize;
  }

  assert(len == 0);
  assert(ptr == range.end());

  return true;
}

/// Return count of items in the protocol iterable.
///
/// If protocol iterable is not valid, return count of valid items.
///
/// \return number of items in iterable.
size_t ofp::detail::ProtocolIteratorFixedTypeItemCount(const ByteRange &range) {
  size_t count = 0;

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  assert((len % 8) == 0);
  assert(IsPtrAligned<8>(ptr));

  while (len > 0) {
    assert(len >= 8);

    size_t elemSize = std::max<size_t>(2, *Big16_cast(ptr));
    assert(elemSize > 0);

    size_t jumpSize = PadLength(elemSize);
    if (jumpSize > len) break;

    len -= jumpSize;
    ptr += jumpSize;
    ++count;
  }

  return count;
}

/// Return true if given ByteRange is a valid protocol iterable for an element
/// with a fixed size.
///
/// To be structurally valid as a ProtocolIterable, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of elemSize
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.
bool ofp::detail::IsProtocolIteratorFixedSizeAndTypeValid(
    size_t elemSize, const ByteRange &range, const char *context) {
  assert(elemSize > 0 && "Elemenent size must not be 0.");
  assert((elemSize % 8) == 0 && "Element size must be multiple of 8.");

  if ((range.size() % elemSize) != 0) {
    log::info("Array element size mismatch:", context);
    return false;
  }

  return true;
}

/// Return count of items in the protocol iterable.
///
/// \return number of items in iterable.
size_t ofp::detail::ProtocolIteratorFixedSizeAndTypeItemCount(
    size_t elemSize, const ByteRange &range) {
  assert(elemSize > 0 && "Element size must not be 0.");
  assert((elemSize % 8) == 0 && "Element size must be multiple of 8.");

  return range.size() / elemSize;
}
