//===- ofp/types.h ----------------------------------------------*- C++ -*-===//
//
// Copyright (c) 2013, Bill Fisher. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Include common headers. Define useful types, functions and macros. 
//
//===----------------------------------------------------------------------===//

#ifndef OFP_TYPES
#define OFP_TYPES

#include <cstddef>				// for std::size_t, etc.
#include <cstdint>				// for std::uint8_t, etc.
#include <cstdlib>				// for std::memcpy, etc.
#include <type_traits>			// for std::make_unsigned<T>, etc.
#include <string>

namespace ofp { // <namespace ofp>

// Metaprogramming Utilities

template<bool B, typename T>
using EnableIf = typename std::enable_if<B,T>::type;

template<bool B, typename T, typename F>
using Conditional = typename std::conditional<B,T,F>::type;

template<typename T>
using MakeUnsigned = typename std::make_unsigned<T>::type;


// Be careful when using UInt8 and UInt16. By default, C++ will promote these
// to an `int` for bitwise and arithmetic operations, so you need to be
// alert for sign extension. Prefer operations against `unsigned int` types.
// See 10.5.1 in TC++PL, 4th ed.
 
using UInt8  = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;

template <class T>
inline constexpr
MakeUnsigned<T> Unsigned_cast(T value) {
	return static_cast<MakeUnsigned<T>>(value);
}

template <class T>
inline constexpr 
UInt8 UInt8_cast(T value) {
	static_assert(sizeof(T) == sizeof(UInt8), "Use UInt8_narrow_cast().");
	return static_cast<UInt8>(Unsigned_cast(value));
}

template <class T>
inline constexpr 
UInt16 UInt16_cast(T value) {
	static_assert(sizeof(T) <= sizeof(UInt16), "Use UInt16_narrow_cast().");
	return static_cast<UInt16>(Unsigned_cast(value));
}

template <class T>
inline constexpr 
UInt32 UInt32_cast(T value) {
	static_assert(sizeof(T) <= sizeof(UInt32), "Use UInt32_narrow_cast().");
	return static_cast<UInt32>(Unsigned_cast(value));
}

template <class T>
inline constexpr
UInt64 UInt64_cast(T value) {
	static_assert(sizeof(T) <= sizeof(UInt64), "No UInt64 narrow casts.");
	return static_cast<UInt64>(Unsigned_cast(value));
}

template <class T>
inline constexpr 
UInt8 UInt8_narrow_cast(T value) {
	static_assert(sizeof(T) != sizeof(UInt8), "Use UInt8_cast().");
	return static_cast<UInt8>(value);
}

template <class T>
inline constexpr 
UInt16 UInt16_narrow_cast(T value) {
	static_assert(sizeof(T) != sizeof(UInt16), "Use UInt16_cast().");
	return static_cast<UInt16>(value);
}

template <class T>
inline constexpr 
UInt16 UInt32_narrow_cast(T value) {
	static_assert(sizeof(T) != sizeof(UInt32), "Use UInt32_cast().");
	return static_cast<UInt32>(value);
}

// Use Unsigned_difference() to subtract unsigned integers.
template <class T>
inline
T Unsigned_difference(T lhs, T rhs) {
	return lhs >= rhs ? lhs - rhs : 0;
}

// Convenience function to check for literal type.
template<typename T>
constexpr bool IsLiteralType() { return std::is_literal_type<T>::value; }

// Convenience function to check for standard layout.
template<typename T>
constexpr bool IsStandardLayout() { return std::is_standard_layout<T>::value; }

// Return the number of items in an array.
template<class T, size_t N>
inline constexpr 
size_t ArrayLength(T (&)[N]) 
{
	return N;
}

// Convert a raw struct/buffer to a hexadecimal string.
std::string RawDataToHex(const void *data, size_t len);
std::string RawDataToHex(const void *data, size_t len, char delimiter, int word = 1);

// Convert a hexadecimal string to a raw struct/buffer. Only write up to maxlen
// bytes. Return the number of bytes written. Set remaining bytes, if any, to 
// zero. Ignore non-hex digits. If the number of hex digits is odd, ignore the 
// last hex digit.
size_t HexToRawData(const std::string &hex, void *data, size_t maxlen);
std::string HexToRawData(const std::string &hex);

// Return true if every byte in block is set to char.
bool MemFilled(const void *data, size_t len, char ch);

} // </namespace ofp>

#endif // OFP_TYPES
