//===- ofp/types.h ----------------------------------------------*- C++ -*-===//
//
// Copyright (c) 2013, Bill Fisher. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Include common headers. Define useful constants, types and functions.
//
//===----------------------------------------------------------------------===//

#ifndef OFP_TYPES_H
#define OFP_TYPES_H

#include <cstddef>     // for std::size_t, etc.
#include <cstdint>     // for std::uint8_t, etc.
#include <cstdlib>     // for std::memcpy, etc.
#include <type_traits> // for std::make_unsigned<T>, etc.
#include <string>
#include <cassert>
#include <ostream>

namespace ofp { // <namespace ofp>

// Metaprogramming Utilities

template <bool B, class T>
using EnableIf = typename std::enable_if<B, T>::type;

template <bool B, class T, class F>
using Conditional = typename std::conditional<B, T, F>::type;

template <class T>
using MakeUnsigned = typename std::make_unsigned<T>::type;

// Be careful when using UInt8 and UInt16. By default, C++ will promote these
// to an `int` for bitwise and arithmetic operations, so you need to be
// alert for sign extension. Prefer operations against `unsigned int` types.
// See 10.5.1 in TC++PL, 4th ed.

using UInt8 = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;

template <class T>
constexpr MakeUnsigned<T> Unsigned_cast(T value)
{
    return static_cast<MakeUnsigned<T>>(value);
}

template <class T>
constexpr UInt8 UInt8_cast(T value)
{
    static_assert(sizeof(T) == sizeof(UInt8), "Use UInt8_narrow_cast().");
    return static_cast<UInt8>(Unsigned_cast(value));
}

template <class T>
constexpr UInt16 UInt16_cast(T value)
{
    static_assert(sizeof(T) <= sizeof(UInt16), "Use UInt16_narrow_cast().");
    return static_cast<UInt16>(Unsigned_cast(value));
}

template <class T>
constexpr UInt32 UInt32_cast(T value)
{
    static_assert(sizeof(T) <= sizeof(UInt32), "Use UInt32_narrow_cast().");
    return static_cast<UInt32>(Unsigned_cast(value));
}

template <class T>
constexpr UInt64 UInt64_cast(T value)
{
    static_assert(sizeof(T) <= sizeof(UInt64), "No UInt64 narrow casts.");
    return static_cast<UInt64>(Unsigned_cast(value));
}

template <class T>
constexpr UInt8 UInt8_narrow_cast(T value)
{
    static_assert(sizeof(T) != sizeof(UInt8), "Use UInt8_cast().");
    return static_cast<UInt8>(value);
}

template <class T>
constexpr UInt16 UInt16_narrow_cast(T value)
{
    static_assert(sizeof(T) != sizeof(UInt16), "Use UInt16_cast().");
    return static_cast<UInt16>(value);
}

template <class T>
constexpr UInt16 UInt32_narrow_cast(T value)
{
    static_assert(sizeof(T) != sizeof(UInt32), "Use UInt32_cast().");
    return static_cast<UInt32>(value);
}

/**
 * Convenience function to compute the difference of two unsigned integers. It
 * prevents underflow or implicit type promotion. If the right argument is
 * greater than the left argument, this function returns zero. This function
 * requires that both arguments are exactly the same unsigned type.
 *
 * @param  lhs left hand side.
 * @param  rhs right hand side.
 * @return difference between lhs and rhs or zero.
 */
template <class T>
constexpr EnableIf<std::is_unsigned<T>::value, T> Unsigned_difference(T lhs,
                                                                      T rhs)
{
    return lhs >= rhs ? lhs - rhs : 0;
}

/**
 * Convenience function to check for literal type. A class with a constexpr
 * constructor is a literal type.
 *
 * @return true if type is a literal type.
 */
template <class T>
constexpr bool IsLiteralType()
{
    return std::is_literal_type<T>::value;
}

/**
 *  Convenience function to check for standard layout.
 *  @return true if type's layout is compatible with C.
 */
template <class T>
constexpr bool IsStandardLayout()
{
    return std::is_standard_layout<T>::value;
}

/**
 *  Return the number of elements in an array.
 *  @return number of elements in array.
 */
template <class T, size_t N>
constexpr size_t ArrayLength(T(&)[N])
{
    return N;
}

/**
 *  Convert raw buffer to a hexadecimal string. The resulting string contains
 *  only hexadecimal characters.
 *  @param  data pointer to input buffer
 *  @param  length size of input buffer
 *  @return hexadecimal string
 */
std::string RawDataToHex(const void* data, size_t length);

/**
 *  Convert raw buffer to a hexadecimal string. The resulting string is
 *  formatted according to `delimiter` and `word`. The delimiter specifies a
 *  character to insert between each run of hexadecimal chars. `word` specifies
 *  the number of bytes between delimiters.
 *
 *  For example, delimiter=':' and word=2 produces "0001:0203:0405"
 *  @param  data pointer to raw memory
 *  @param  length size of input buffer
 *  @param  delimiter character to insert to format the hex string
 *  @param  word number of bytes between delimiters
 *  @return hexadecimal string
 */
std::string RawDataToHex(const void* data, size_t length, char delimiter,
                         int word = 1);

/**
 *  Convert a hexadecimal string to raw memory. Only write up to `length` bytes.
 *  Ignore non-hex digits and the odd final hex digit. If there are fewer than
 *  `length` bytes converted from the hex string, set the remaining bytes to
 *  zero.
 *  @param  hex string containing hexadecimal characters
 *  @param  data output buffer
 *  @param  length size of output buffer
 *  @return number of bytes resulting from hexadecimal string
 */
size_t HexToRawData(const std::string& hex, void* data, size_t length);

/**
 *  Convert a hexadecimal string to raw memory. Ignore non-hex digits and the
 *  odd final hex digit.
 *  @param  hex string containing hexadecimal characters
 *  @return string containing raw bytes
 */
std::string HexToRawData(const std::string& hex);

/**
 *  Return true if memory block is filled with given byte value.
 *  @param  data pointer to memory block
 *  @param  length length of memory block
 *  @param  fill fill byte
 *  @return true if memory block is filled with given byte value.
 */
bool IsMemFilled(const void* data, size_t length, char fill);

} // </namespace ofp>

#endif // OFP_TYPES_H
