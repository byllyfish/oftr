//===- ofp/byteorder.h ------------------------------------------*- C++ -*-===//
//
// Copyright (c) 2013, Bill Fisher. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Define integral types for dealing with host byteorder.
//
//===----------------------------------------------------------------------===//

#ifndef OFP_BYTEORDER_H
#define OFP_BYTEORDER_H

#include "ofp/types.h"

#if defined(__linux__) || defined(__GNU__)
# include <endian.h>
#elif !defined(BYTE_ORDER)
# include <machine/endian.h>
#endif

namespace ofp { // <namespace ofp>
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
inline constexpr
UInt16 SwapTwoBytes(UInt16 n) 
{
	return UInt16_narrow_cast(((n & 0x00ffU) << 8) | 
	    			   		  ((n & 0xff00U) >> 8));
}

/// \return 32-bit value with byte order swapped.
inline constexpr
UInt32 SwapFourBytes(UInt32 n)
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
inline constexpr
UInt64 SwapEightBytes(UInt64 n) 
{
# ifdef __llvm__
	return __builtin_bswap64(n);
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

template <class Type>
inline constexpr
EnableIf<sizeof(Type) == sizeof(UInt8), Type> SwapByteOrder(Type n) 
{
	return n;
}

template <class Type>
inline constexpr
EnableIf<sizeof(Type) == sizeof(UInt16), Type> SwapByteOrder(Type n) 
{
	return static_cast<Type>(SwapTwoBytes(UInt16_cast(n)));
}

template <class Type>
inline constexpr
EnableIf<sizeof(Type) == sizeof(UInt32), Type> SwapByteOrder(Type n)
{
	return static_cast<Type>(SwapFourBytes(UInt32_cast(n)));
}

template <class Type>
inline constexpr
EnableIf<sizeof(Type) == sizeof(UInt64), Type> SwapByteOrder(Type n)
{
	return static_cast<Type>(SwapEightBytes(UInt64_cast(n)));
}


template <class Type>
inline constexpr
Type HostSwapByteOrder(Type n)
{
	return IsHostLittleEndian ? SwapByteOrder(n) : n;
}

/// \brief Concrete class for big-endian integer types (aligned).
template <class Type>
class BigEndianAligned {
public:
	using NativeType = Type;

	constexpr BigEndianAligned() = default;
	constexpr BigEndianAligned(Type n) : n_{ HostSwapByteOrder(n) } {}
	
	constexpr operator Type() const { 
		return HostSwapByteOrder(n_);
	}

	void operator=(Type n) {
		n_ = HostSwapByteOrder(n);
	}

private:
	Type n_;
};

template <class Type>
class BigEndianUnaligned {
public:
	// TODO: add constexpr constructors.

	operator Type() const { 
		Type n;
		std::memcpy(&n, b_, sizeof(n));
		return HostSwapByteOrder(n);
	}

	void operator=(Type n) {
		Type v = HostSwapByteOrder(n);
		std::memcpy(b_, &v, sizeof(b_));
	}

private:
	alignas(1) UInt8 b_[sizeof(Type)];
};


template <class Type>
struct NativeTypeOf {

	template <class X>
	static typename X::NativeType check(const X &x);
	
	static Type check(...);
	
	using type = decltype(check(std::declval<Type>()));
	
};


} // </namespace detail>

/// \brief Types for big endian integers (aligned).

template <class T>
using Big = detail::BigEndianAligned<T>;

using Big8  = UInt8;
using Big16 = Big<UInt16>;
using Big32 = Big<UInt32>;
using Big64 = Big<UInt64>;

/// \brief Types for big endian integers (unaligned).

template <class T>
using Big_unaligned = detail::BigEndianUnaligned<T>;

// For compile-time conversions.
template <class Type>
constexpr inline
EnableIf<std::is_integral<Type>::value, Type> BigEndianFromNative(Type value)
{
	return detail::HostSwapByteOrder(value);
}

template <class Type>
constexpr inline
EnableIf<std::is_integral<Type>::value, Type> BigEndianToNative(Type value)
{
	return detail::HostSwapByteOrder(value);
}

template <class T>
using NativeTypeOf = detail::NativeTypeOf<T>;

} // </namespace ofp>

#endif //OFP_BYTEORDER_H
