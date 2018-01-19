// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/smallbuffer.h"
#include "ofp/padding.h"

using ofp::SmallBuffer;
using ofp::UInt8;

SmallBuffer::SmallBuffer(SmallBuffer &&buf) noexcept {
  buf.assertInvariant();

  if (buf.isSmall()) {
    size_t len = buf.size();
    std::memcpy(buf_, buf.buf_, len);
    begin_ = buf_;
    end_ = begin_ + len;
    capacity_ = begin_ + IntrinsicBufSize;
  } else {
    begin_ = buf.begin_;
    end_ = buf.end_;
    capacity_ = buf.capacity_;
  }
  // We're done with buf.
  buf.init();
}

SmallBuffer &SmallBuffer::operator=(const SmallBuffer &buf) noexcept {
  assertInvariant();
  buf.assertInvariant();

  assign(buf.begin(), buf.size());
  return *this;
}

SmallBuffer &SmallBuffer::operator=(SmallBuffer &&buf) noexcept {
  assertInvariant();
  buf.assertInvariant();

  if (buf.isSmall()) {
    assign(buf.begin(), buf.size());
  } else {
    if (!isSmall()) {
      std::free(begin_);
    }
    begin_ = buf.begin_;
    end_ = buf.end_;
    capacity_ = buf.capacity_;
  }
  // We're done with buf.
  buf.init();
  return *this;
}

void SmallBuffer::assign(const void *data, size_t length) noexcept {
  assertNoOverlap(data, length);
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (length > capacity()) {
    increaseCapacity(length);
  }

  std::memcpy(begin_, data, length);
  end_ = begin_ + length;
}

void SmallBuffer::insert(UInt8 *pos, const void *data, size_t length) noexcept {
  assertInRange(pos);
  assertNoOverlap(data, length);
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (length > remaining()) {
    size_t posOffset = Unsigned_cast(pos - begin_);
    increaseCapacity(size() + length);
    pos = begin_ + posOffset;

    assertInRange(pos);
  }

  std::memmove(pos + length, pos, Unsigned_cast(end_ - pos));
  std::memcpy(pos, data, length);
  end_ += length;
}

void SmallBuffer::replace(UInt8 *pos, UInt8 *posEnd, const void *data,
                          size_t length) noexcept {
  assertInRange(pos, posEnd);
  assertNoOverlap(data, length);
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  size_t posLen = Unsigned_cast(posEnd - pos);
  if (length > posLen) {
    size_t more = length - posLen;
    if (more > remaining()) {
      size_t posOffset = Unsigned_cast(pos - begin_);
      size_t posLength = Unsigned_cast(posEnd - pos);
      increaseCapacity(size() + more);
      pos = begin_ + posOffset;
      posEnd = pos + posLength;

      assertInRange(pos, posEnd);
    }
    std::memmove(posEnd + more, posEnd, Unsigned_cast(end_ - posEnd));
    std::memcpy(pos, data, length);
    end_ += more;
  } else {
    size_t less = posLen - length;
    std::memmove(posEnd - less, posEnd, Unsigned_cast(end_ - posEnd));
    std::memcpy(pos, data, length);
    end_ -= less;
  }
}

void SmallBuffer::remove(UInt8 *pos, UInt8 *posEnd) noexcept {
  assertInRange(pos, posEnd);
  assertInvariant();

  size_t less = Unsigned_cast(posEnd - pos);
  std::memmove(posEnd - less, posEnd, Unsigned_cast(end_ - posEnd));
  end_ -= less;

  assertInvariant();
}

void SmallBuffer::resize(size_t length) noexcept {
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (length > capacity()) {
    increaseCapacity(length);
  }
  end_ = begin_ + length;
}

void SmallBuffer::reset(size_t length) noexcept {
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (!isSmall()) {
    std::free(begin_);
  }

  if (length <= IntrinsicBufSize) {
    begin_ = buf_;
    end_ = begin_ + length;
    capacity_ = begin_ + IntrinsicBufSize;

  } else {
    size_t newCapacity = PadLength(length);
    UInt8 *newBuf = static_cast<UInt8 *>(std::malloc(newCapacity));
    if (!newBuf) {
      log::fatal("ofp::SmallBuffer: malloc failed:", newCapacity);
    }
    begin_ = newBuf;
    end_ = begin_ + length;
    capacity_ = begin_ + newCapacity;
  }

  assertInvariant();
}

UInt8 *SmallBuffer::addUninitialized(size_t length) noexcept {
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (length > remaining()) {
    increaseCapacity(size() + length);
  }

  UInt8 *pos = end_;
  end_ += length;
  return pos;
}

UInt8 *SmallBuffer::insertUninitialized(UInt8 *pos, size_t length) noexcept {
  assertInRange(pos);
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  if (length > remaining()) {
    size_t posOffset = Unsigned_cast(pos - begin_);
    increaseCapacity(size() + length);
    pos = begin_ + posOffset;

    assertInRange(pos);
  }

  std::memmove(pos + length, pos, Unsigned_cast(end_ - pos));
  end_ += length;
  return pos;
}

UInt8 *SmallBuffer::replaceUninitialized(UInt8 *pos, UInt8 *posEnd,
                                         size_t length) noexcept {
  assertInRange(pos, posEnd);
  assertInvariant();
  assert(length <= 0xFFFFFFFF);

  size_t posLen = Unsigned_cast(posEnd - pos);
  if (length > posLen) {
    size_t more = length - posLen;
    if (more > remaining()) {
      size_t posOffset = Unsigned_cast(pos - begin_);
      size_t posLength = Unsigned_cast(posEnd - pos);
      increaseCapacity(more);
      pos = begin_ + posOffset;
      posEnd = pos + posLength;

      assertInRange(pos, posEnd);
    }
    std::memmove(posEnd + more, posEnd, Unsigned_cast(end_ - posEnd));
    end_ += more;

  } else {
    size_t less = posLen - length;
    std::memmove(posEnd - less, posEnd, Unsigned_cast(end_ - posEnd));
    end_ -= less;
  }
  return pos;
}

void SmallBuffer::addZeros(size_t length) noexcept {
  UInt8 *p = addUninitialized(length);
  std::memset(p, 0, length);
}

void SmallBuffer::insertZeros(UInt8 *pos, size_t length) noexcept {
  UInt8 *p = insertUninitialized(pos, length);
  std::memset(p, 0, length);
}

void SmallBuffer::replaceZeros(UInt8 *pos, UInt8 *posEnd,
                               size_t length) noexcept {
  UInt8 *p = replaceUninitialized(pos, posEnd, length);
  std::memset(p, 0, length);
}

void SmallBuffer::increaseCapacity(size_t newLength) noexcept {
  assertInvariant();

  size_t newCapacity = computeCapacity(newLength);
  size_t len = size();
  UInt8 *newBuf = nullptr;

  assert(newCapacity > capacity());

  if (isSmall()) {
    // Allocate a new larger buffer, and copy the contents of the intrinsic
    // buffer to the new buffer.

    newBuf = static_cast<UInt8 *>(std::malloc(newCapacity));
    if (!newBuf) {
      log::fatal("ofp::SmallBuffer: malloc failed:", newCapacity);
    }

    std::memcpy(newBuf, begin_, len);

  } else {
    // Allocate a new larger buffer with contents copied from the current
    // buffer block.

    newBuf = static_cast<UInt8 *>(std::realloc(begin_, newCapacity));
    if (!newBuf) {
      log::fatal("ofp::SmallBuffer: realloc failed:", newCapacity);
    }
  }

  begin_ = newBuf;
  end_ = begin_ + len;
  capacity_ = begin_ + newCapacity;

  assertInvariant();
}

size_t SmallBuffer::computeCapacity(size_t length) noexcept {
  assert(length > IntrinsicBufSize);

  if (length <= 1024)
    return 1024;
  if (length <= 8192)
    return 8192;
  if (length <= 65536)
    return 65536;
  if (length <= 524288)
    return 524288;
  if (length <= 4194304)
    return 4194304;

  return 2 * PadLength(length);
}
