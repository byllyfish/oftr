#include "ofp/protocoliterator.h"
#include "ofp/log.h"
#include <algorithm>

bool ofp::detail::IsProtocolRangeValid(const ByteRange &range,
                                          size_t sizeFieldOffset,
                                          const char *context,
                                          size_t minElemSize) {
  assert(sizeFieldOffset <= detail::ProtocolIteratorSizeOffsetCutoff);

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

    UInt16 elemSize = *Big16_cast(ptr + sizeFieldOffset);
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

size_t ofp::detail::ProtocolRangeItemCount(const ByteRange &range, size_t sizeFieldOffset) {
  assert(sizeFieldOffset <= detail::ProtocolIteratorSizeOffsetCutoff);

  size_t count = 0;

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  assert((len % 8) == 0);
  assert(IsPtrAligned<8>(ptr));

  const size_t minSize = sizeFieldOffset + sizeof(Big16);

  while (len > 0) {
    assert(len >= 8);

    size_t elemSize = std::max<size_t>(minSize, *Big16_cast(ptr + sizeFieldOffset));
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
/// To be structurally valid as a ProtocolRange, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of elemSize
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.
bool ofp::detail::IsProtocolRangeFixedValid(
    size_t elemSize, const ByteRange &range, const char *context) {
  assert(elemSize > 0 && "Element size must not be 0.");
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
size_t ofp::detail::ProtocolRangeFixedItemCount(
    size_t elemSize, const ByteRange &range) {
  assert(elemSize > 0 && "Element size must not be 0.");
  assert((elemSize % 8) == 0 && "Element size must be multiple of 8.");

  return range.size() / elemSize;
}