#include "ofp/protocoliterator.h"
#include "ofp/log.h"
#include <algorithm>

using namespace ofp;


/// Return true if given ByteRange is a valid protocol iterable for an element
/// with a fixed size.
/// 
/// To be structurally valid as a ProtocolRange, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of elemSize
///   - pointer to start of range is 8-byte aligned.
/// 
/// \return true if byte range is a valid protocol iterable.
static bool isProtocolRangeFixedValid(size_t elementSize, const ByteRange &range, const char *context="");

/// Return count of items in the protocol iterable.
/// 
/// \return number of items in iterable.
static size_t protocolRangeFixedItemCount(size_t elementSize, const ByteRange &range);


bool ofp::detail::IsProtocolRangeValid(size_t elementSize,
                                          const ByteRange &range,
                                          size_t sizeFieldOffset,
                                          size_t alignment,
                                          const char *context) {

  if (sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_FIXED) {
    return isProtocolRangeFixedValid(elementSize, range, context);
  }

  assert(sizeFieldOffset < 32 || sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_CONDITIONAL);

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  // Length must be a multiple of `alignment`.
  if ((len % alignment) != 0) {
    log::info("Array length is not a multiple of Alignment:", context);
    return false;
  }

  // Beginning pointer must be aligned.
  if (!IsPtrAligned(ptr, alignment)) {
    log::info("Array start is not aligned:", context);
    return false;
  }

  // All of the element lengths must add up (when padded).
  while (len > 0) {
    assert(len >= alignment);

    UInt16 elemSize;
    if (sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_CONDITIONAL) {
      elemSize = *Big16_cast(ptr) == 0xffff ? 8 : 4;
    } else {
      elemSize = *Big16_cast(ptr + sizeFieldOffset);
    }

    if (elemSize < 4) {
      log::info("Array element size less than minimum:", context);
      return false;
    }

    // If alignment isn't 8-bytes, the element size must be a multiple of the 
    // alignment. If the alignment is 8-bytes, we just pad out the element size
    // to a multiple of 8.

    size_t jumpSize;
    if (alignment != 8) {
      if ((elemSize % alignment) != 0) {
        log::info("Array element size is not a multiple of Alignment:", context);
        return false;
      }
      jumpSize = elemSize;
    } else {
      assert(alignment == 8);
      jumpSize = PadLength(elemSize);
    }

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

size_t ofp::detail::ProtocolRangeItemCount(size_t elementSize, const ByteRange &range, size_t sizeFieldOffset, size_t alignment) {
  if (sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_FIXED) {
    return protocolRangeFixedItemCount(elementSize, range);
  }

  assert(sizeFieldOffset < 32 || sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_CONDITIONAL);
  assert(alignment == 4 || alignment == 8);
  
  size_t count = 0;

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  assert((len % alignment) == 0);
  assert(IsPtrAligned(ptr, alignment));

  const size_t minSize = sizeFieldOffset + sizeof(Big16);

  while (len > 0) {
    assert(len >= alignment);

    size_t elemSize;
    if (sizeFieldOffset == PROTOCOL_ITERATOR_SIZE_CONDITIONAL) {
      assert(alignment == 4);
      elemSize = *Big16_cast(ptr) == 0xffff ? 8 : 4;
    } else {
      elemSize = std::max<size_t>(minSize, *Big16_cast(ptr + sizeFieldOffset));
      assert(elemSize > 0);
    }

    size_t jumpSize = (alignment == 8) ? PadLength(elemSize) : elemSize;
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
static bool isProtocolRangeFixedValid(
    size_t elementSize, const ByteRange &range, const char *context) {
  assert(elementSize > 0 && "Element size must not be 0.");
  assert((elementSize % 8) == 0 && "Element size must be multiple of 8.");

  if ((range.size() % elementSize) != 0) {
    log::info("Array element size mismatch:", context);
    return false;
  }

  return true;
}

/// Return count of items in the protocol iterable.
///
/// \return number of items in iterable.
static size_t protocolRangeFixedItemCount(
    size_t elementSize, const ByteRange &range) {
  assert(elementSize > 0 && "Element size must not be 0.");
  assert((elementSize % 8) == 0 && "Element size must be multiple of 8.");

  return range.size() / elementSize;
}
