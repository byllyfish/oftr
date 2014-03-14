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

  void append(const void *data, size_t length) noexcept;

  void insert(UInt8 *pos, const void *data, size_t length) noexcept;

  void replace(UInt8 *pos, UInt8 *posEnd, const void *data,
               size_t length) noexcept;

  void remove(UInt8 *pos, UInt8 *posEnd) noexcept;

  void resize(size_t length) noexcept;

  void clear() noexcept { end_ = begin_; }

  void reset(size_t length) noexcept;

  void addUninitialized(size_t length) noexcept;

  void insertUninitialized(UInt8 *pos, size_t length) noexcept;

  void replaceUninitialized(UInt8 *pos, UInt8 *posEnd, size_t length) noexcept;

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

  void assertNoOverlap(const void *data, size_t length) const noexcept {
    assert((BytePtr(data) + length <= begin_ || BytePtr(data) >= capacity_) &&
           "data overlaps with buffer");
  }
};

}  // namespace ofp
