#ifndef OFP_SMALLBUFFER_H_
#define OFP_SMALLBUFFER_H_

#include "ofp/log.h"

namespace ofp {

/// \brief Concrete class for a memory buffer that does not require memory
/// allocation for small buffers; the data is stored in a fixed-size intrinsic
/// array.
///
/// A SmallBuffer uses malloc/realloc to allocate memory when its content grows
/// beyond its intrinsic size. The maximum size of a SmallBuffer is 65535 bytes.

class SmallBuffer {
 public:
  SmallBuffer() noexcept : begin_{buf_},
                           end_{begin_},
                           capacity_{begin_ + IntrinsicBufSize} {}

  SmallBuffer(const void *data, size_t length) noexcept : SmallBuffer{} {
    assign(data, length);
  }

  SmallBuffer(const SmallBuffer &buf) noexcept
      : SmallBuffer{buf.begin(), buf.size()} {}

  SmallBuffer(SmallBuffer &&buf) noexcept;

  ~SmallBuffer();

  SmallBuffer &operator=(const SmallBuffer &buf) noexcept;

  SmallBuffer &operator=(SmallBuffer &&buf) noexcept;

  bool operator==(const SmallBuffer &buf) const noexcept {
    return size() == buf.size() &&
           std::memcmp(begin(), buf.begin(), size()) == 0;
  }

  bool operator!=(const SmallBuffer &buf) const noexcept {
    return !operator==(buf);
  }

  UInt8 *begin() noexcept { return begin_; }
  UInt8 *end() noexcept { return end_; }

  const UInt8 *begin() const noexcept { return begin_; }
  const UInt8 *end() const noexcept { return end_; }

  size_t size() const noexcept { return Unsigned_cast(end_ - begin_); }
  size_t capacity() const noexcept { return Unsigned_cast(capacity_ - begin_); }

  void assign(const void *data, size_t length) noexcept;

  void add(const void *data, size_t length) noexcept;

  void insert(UInt8 *pos, const void *data, size_t length) noexcept;

  void replace(UInt8 *pos, UInt8 *posEnd, const void *data,
               size_t length) noexcept;

  void remove(UInt8 *pos, UInt8 *posEnd) noexcept;

  void resize(size_t length) noexcept;

  void clear() noexcept { end_ = begin_; }

  void reset(size_t length) noexcept;

  UInt8 *addUninitialized(size_t length) noexcept;

  UInt8 *insertUninitialized(UInt8 *pos, size_t length) noexcept;

  UInt8 *replaceUninitialized(UInt8 *pos, UInt8 *posEnd, size_t length) noexcept;

  void addZeros(size_t length) noexcept;

  void insertZeros(UInt8 *pos, size_t length) noexcept;

  void replaceZeros(UInt8 *pos, UInt8 *posEnd, size_t length) noexcept;

 private:
  enum : size_t {
    IntrinsicBufSize = 64
  };

  UInt8 *begin_;
  UInt8 *end_;
  UInt8 *capacity_;
  OFP_ALIGNAS(8) UInt8 buf_[IntrinsicBufSize];

  size_t remaining() const noexcept { return Unsigned_cast(capacity_ - end_); }

  /// \returns True if buffer space is located within object rather than by
  /// malloc.
  bool isSmall() const noexcept { return begin_ == buf_; }

  void increaseCapacity(size_t newLength) noexcept;

  static size_t computeCapacity(size_t length) noexcept;

  /// \brief Helper function to verify pointer is valid for byte buffer.
  void assertInRange(const UInt8 *pos) const noexcept {
    assert((pos != nullptr) && "position is null");
    assert((pos >= begin_ && pos <= end_) && "position out of range");
  }

  void assertInRange(const UInt8 *pos, const UInt8 *posEnd) {
    assertInRange(pos);
    assertInRange(posEnd);
    assert(pos <= posEnd);
  }

  void assertNoOverlap(const void *data, size_t length) const noexcept {
    assert((BytePtr(data) + length <= begin_ || BytePtr(data) >= capacity_) &&
           "data overlaps with buffer");
  }

  void assertInvariant() const noexcept {
    assert(begin_ <= end_);
    assert(end_ <= capacity_);
    assert((begin_ != buf_) || (capacity_ == begin_ + IntrinsicBufSize));
  }
};

inline SmallBuffer::~SmallBuffer() {
  if (!isSmall()) {
    std::free(begin_);
  }
}

inline void SmallBuffer::add(const void *data, size_t length) noexcept {
  assertNoOverlap(data, length);
  assertInvariant();

  if (length > remaining()) {
    increaseCapacity(size() + length);
  }

  std::memcpy(end_, data, length);
  end_ += length;
}

}  // namespace ofp

#endif // OFP_SMALLBUFFER_H_
