// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BYTEORDER_H_
#define OFP_BYTEORDER_H_

#if defined(__linux__) || defined(__GNU__)
#include <endian.h>
#elif !defined(BYTE_ORDER)
#include <machine/endian.h>
#endif
#include "ofp/types.h"

namespace ofp {
namespace detail {

/// True if host is big endian.
#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER == BIG_ENDIAN
constexpr bool IsHostBigEndian = true;
#else
constexpr bool IsHostBigEndian = false;
#endif

/// True if host is little endian.
constexpr bool IsHostLittleEndian = !IsHostBigEndian;

/// \return 16-bit value with byte order swapped.
constexpr UInt16 SwapTwoBytes(UInt16 n) {
  return UInt16_narrow_cast(((n & 0x00ffU) << 8) | ((n & 0xff00U) >> 8));
}

/// \return 32-bit value with byte order swapped.
constexpr UInt32 SwapFourBytes(UInt32 n) {
#ifdef __llvm__
  return __builtin_bswap32(n);
#else
  return ((n & 0x000000ffUL) << 24) | ((n & 0x0000ff00UL) << 8) |
         ((n & 0x00ff0000UL) >> 8) | ((n & 0xff000000UL) >> 24);
#endif
}

/// \return 64-bit value with byte order swapped.
constexpr UInt64 SwapEightBytes(UInt64 n) {
#ifdef __llvm__
  return __builtin_bswap64(n);
#else
  return ((n & 0x00000000000000ffULL) << 56) |
         ((n & 0x000000000000ff00ULL) << 40) |
         ((n & 0x0000000000ff0000ULL) << 24) |
         ((n & 0x00000000ff000000ULL) << 8) |
         ((n & 0x000000ff00000000ULL) >> 8) |
         ((n & 0x0000ff0000000000ULL) >> 24) |
         ((n & 0x00ff000000000000ULL) >> 40) |
         ((n & 0xff00000000000000ULL) >> 56);
#endif
}

/// \return Integer with byte order swapped.

template <class Type>
constexpr EnableIf<sizeof(Type) == sizeof(UInt8), Type> SwapByteOrder(Type n) {
  return n;
}

template <class Type>
constexpr EnableIf<sizeof(Type) == sizeof(UInt16), Type> SwapByteOrder(Type n) {
  return static_cast<Type>(SwapTwoBytes(UInt16_cast(n)));
}

template <class Type>
constexpr EnableIf<sizeof(Type) == sizeof(UInt32), Type> SwapByteOrder(Type n) {
  return static_cast<Type>(SwapFourBytes(UInt32_cast(n)));
}

template <class Type>
constexpr EnableIf<sizeof(Type) == sizeof(UInt64), Type> SwapByteOrder(Type n) {
  return static_cast<Type>(SwapEightBytes(UInt64_cast(n)));
}

template <class Type>
constexpr Type HostSwapByteOrder(Type n) {
  return IsHostLittleEndian ? SwapByteOrder(n) : n;
}

/// \brief Concrete class for big-endian integer types (aligned).
template <class Type>
class BigEndianAligned {
 public:
  using NativeType = Type;

  constexpr BigEndianAligned() : n_{static_cast<Type>(0)} {}
  constexpr BigEndianAligned(Type n) : n_{HostSwapByteOrder(n)} {}
  constexpr BigEndianAligned(const BigEndianAligned &n) = default;

  constexpr operator Type() const { return HostSwapByteOrder(n_); }
  constexpr bool operator!() const { return !n_; }

  void operator=(Type n) { n_ = HostSwapByteOrder(n); }
  BigEndianAligned &operator=(const BigEndianAligned &n) = default;

  static BigEndianAligned fromBytes(const UInt8 *data) {
    BigEndianAligned big;
    std::memcpy(&big, data, sizeof(big));
    return big;
  }

  /// Copy data from given buffer into aligned big endian object. If there is
  /// more than enough data, only copy the trailing bytes.
  static BigEndianAligned fromBytes(const UInt8 *data, size_t size) {
    BigEndianAligned big;
    if (size == 0)
      return BigEndianAligned{};
    if (size > sizeof(big)) {
      return fromBytes(data + size - sizeof(big));
    }
    std::memcpy(MutableBytePtr(&big) + sizeof(big) - size, data, size);
    return big;
  }

 private:
  OFP_ALIGNAS(sizeof(Type)) Type n_;
};

template <class Type>
struct NativeTypeOf {
  template <class X>
  static typename X::NativeType check(const X &x);

  static Type check(...);

  using type = decltype(check(std::declval<Type>()));
};

}  // namespace detail

/// \brief Types for big endian integers (aligned).

template <class T>
using Big = detail::BigEndianAligned<T>;

using Big8 = Big<UInt8>;
using Big16 = Big<UInt16>;
using Big32 = Big<UInt32>;
using Big64 = Big<UInt64>;
using SignedBig32 = Big<SignedInt32>;

static_assert(IsLiteralType<Big8>(), "Literal type expected.");
static_assert(IsLiteralType<Big16>(), "Literal type expected.");
static_assert(IsLiteralType<Big32>(), "Literal type expected.");
static_assert(IsLiteralType<Big64>(), "Literal type expected.");
static_assert(IsLiteralType<SignedBig32>(), "Literal type expected.");

/// Literal Type representing a 24-bit unsigned integer in big-endian layout.
class Big24 {
 public:
  using NativeType = UInt32;

  constexpr Big24() : n_{0, 0, 0} {}
  constexpr Big24(UInt32 n)
      : n_{UInt8_narrow_cast((n >> 16) & 0xFF),
           UInt8_narrow_cast((n >> 8) & 0xFF), UInt8_narrow_cast(n & 0xFF)} {}

  constexpr operator UInt32() const {
    return (UInt32_cast(n_[0]) << 16) | (UInt32_cast(n_[1]) << 8) |
           UInt32_cast(n_[2]);
  }
  constexpr bool operator!() const { return !n_[0] || !n_[1] || !n_[0]; }

 private:
  UInt8 n_[3];
};

static_assert(sizeof(Big24) == 3, "Unexpected size.");
static_assert(IsLiteralType<Big24>(), "Literal type expected.");

// For compile-time conversions.
template <class Type>
constexpr EnableIf<std::is_integral<Type>::value, Type> BigEndianFromNative(
    Type value) {
  return detail::HostSwapByteOrder(value);
}

template <class Type>
constexpr EnableIf<std::is_integral<Type>::value, Type> BigEndianToNative(
    Type value) {
  return detail::HostSwapByteOrder(value);
}

template <class T>
using NativeTypeOf = detail::NativeTypeOf<T>;

// Casting Convenience Functions

inline const Big16 *Big16_cast(const void *ptr) {
  assert(IsPtrAligned(ptr, 2) && "Big16_cast failed");
  return reinterpret_cast<const Big16 *>(ptr);
}

inline Big16 *Big16_cast(void *ptr) {
  assert(IsPtrAligned(ptr, 2) && "Big16_cast failed");
  return reinterpret_cast<Big16 *>(ptr);
}

inline const Big32 *Big32_cast(const void *ptr) {
  assert(IsPtrAligned(ptr, 4) && "Big32_cast failed");
  return reinterpret_cast<const Big32 *>(ptr);
}

inline Big32 *Big32_cast(void *ptr) {
  assert(IsPtrAligned(ptr, 4) && "Big32_cast failed");
  return reinterpret_cast<Big32 *>(ptr);
}

// Copy Functions don't require the source to be aligned

inline const Big16 Big16_unaligned(const void *ptr) {
  return Big16::fromBytes(BytePtr(ptr));
}

inline const Big32 Big32_unaligned(const void *ptr) {
  return Big32::fromBytes(BytePtr(ptr));
}

}  // namespace ofp

#endif  // OFP_BYTEORDER_H_
