//===- ofp/byteorder.h ------------------------------------------*- C++ -*-===//
//
// Copyright (c) 2013, Bill Fisher. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Define integral types for dealing with host byteorder.
//
//   ubig8_t
//   ubig16_t
//   ubig32_t
//   ubig64_t
//
//===----------------------------------------------------------------------===//

#ifndef OFP_BYTEORDER_H
#define OFP_BYTEORDER_H

#include <cstdint>
#if defined(__linux__) || defined(__GNU__)
# include <endian.h>
#elif !defined(BYTE_ORDER)
# include <machine/endian.h>
#endif

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>
namespace detail { // <namespace detail> 

/// \brief True if host is big endian.
#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER == BIG_ENDIAN
  constexpr bool IsHostBigEndian = true;
#else
  constexpr bool IsHostBigEndian = false;
#endif

/// \brief True if host is little endian.
constexpr bool IsHostLittleEndian = !IsHostBigEndian;

/// \return 16-bit value with byte order swapped.
inline
std::uint16_t SwapTwoBytes(std::uint16_t n) 
{
# ifdef __llvm__
	return __builtin_bswap16(n);
# else
	return ((n & 0x00ffU) << 8) | 
	       ((n & 0xff00U) >> 8);
# endif
}

/// \return 32-bit value with byte order swapped.
inline
std::uint32_t SwapFourBytes(std::uint32_t n)
{
# ifdef __llvm__
	return __builtin_bswap32(n);
# else
	return ((n & 0x000000ffUL) << 24) | 
		   ((n & 0x0000ff00UL) <<  8) | 
		   ((n & 0x00ff0000UL) >>  8) | 
		   ((n & 0xff000000UL) >> 24);
# endif
}

/// \return 64-bit value with byte order swapped.
inline
std::uint64_t SwapEightBytes(std::uint64_t n) 
{
# ifdef __llvm__
	return __builtin__bswap64(n);
# else
	return ((n & 0x00000000000000ffULL) << 56) |
		   ((n & 0x000000000000ff00ULL) << 40) |
		   ((n & 0x0000000000ff0000ULL) << 24) |
		   ((n & 0x00000000ff000000ULL) <<  8) |
	       ((n & 0x000000ff00000000ULL) >>  8) |
		   ((n & 0x0000ff0000000000ULL) >> 24) |
		   ((n & 0x00ff000000000000ULL) >> 40) |
		   ((n & 0xff00000000000000ULL) >> 56);
# endif
}

/// \return Integer with byte order swapped.

inline
std::uint16_t SwapByteOrder(std::uint16_t n)
{
	return SwapTwoBytes(n);
}

inline
std::uint32_t SwapByteOrder(std::uint32_t n)
{
	return SwapFourBytes(n);
}

inline
std::uint64_t SwapByteOrder(std::uint64_t n)
{
	return SwapEightBytes(n);
}

/// \brief Concrete class for big-endian integers (aligned).
template <class IntType>
class BigEndianInt {
public:
	operator IntType() const { 
		return IsHostLittleEndian ? SwapByteOrder(n_) : n_;
	}

	void operator=(IntType n) {
		n_ = IsHostLittleEndian ? SwapByteOrder(n) : n;
	}

private:
	IntType n_;
};

} // </namespace detail>

/// \brief Types for big endian integers (aligned).

using ubig8_t  = uint8_t;
using ubig16_t = detail::BigEndianInt<std::uint16_t>;
using ubig32_t = detail::BigEndianInt<std::uint32_t>;
using ubig64_t = detail::BigEndianInt<std::uint64_t>;

} // </namespace sys>
} // </namespace ofp>

#endif //OFP_BYTEORDER_H
