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
#include <string>

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

//using std::size_t;
//using std::uint8_t;
//using std::uint16_t;
//using std::uint32_t;
//using std::uint64_t;

template <class T>
inline constexpr 
uint8_t uint8_cast(T v) {
	return static_cast<uint8_t>(v);
}

template <class T>
inline constexpr 
uint16_t uint16_cast(T v) {
	return static_cast<uint16_t>(v);
}

template <class T>
inline constexpr 
uint32_t uint32_cast(T v) {
	return static_cast<uint32_t>(v);
}

template <class T>
inline constexpr
uint64_t uint64_cast(T v) {
	return static_cast<uint64_t>(v);
}

// Return the number of items in an array.
template<class T, size_t N>
inline constexpr 
size_t ArrayLength(T (&)[N]) 
{
	return N;
}

// Convert a raw struct/buffer to a hexadecimal string.
std::string RawDataToHex(const void *data, size_t len);

// Convert a hexadecimal string to a raw struct/buffer. Only write up to maxlen
// bytes. Return the number of bytes used. Set remaining bytes, if any, to 
// zero. Ignore non-hex digits. If the number of hex digits is odd, ignore the 
// last hex digit.
size_t HexToRawData(const std::string &hex, void *data, size_t maxlen);

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_TYPES
