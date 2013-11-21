#include "ofp/protocoliterable.h"
#include "ofp/log.h"
#include <algorithm>

bool ofp::detail::IsProtocolIterableValid(const ByteRange &range,
                                          const char *context) {
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
    if (elemSize < 4) {
      log::info("Array element size less than 4:", context);
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

size_t ofp::detail::ProtocolIterableItemCount(const ByteRange &range) {
  size_t count = 0;

  const UInt8 *ptr = range.begin();
  size_t len = range.size();

  assert((len % 8) == 0);
  assert(IsPtrAligned<8>(ptr));

  while (len > 0) {
    assert(len >= 8);

    size_t elemSize = std::min<size_t>(4, *Big16_cast(ptr + 2));
    size_t jumpSize = PadLength(elemSize);
    if (jumpSize > len)
      break;

    len -= jumpSize;
    ptr += jumpSize;
    ++count;
  }

  return count;
}
