// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TYPES_H_
#define OFP_TYPES_H_

#include <cassert>  // for assert macro
#include <cctype>   // for std::isxdigit, etc.
#include <chrono>
#include <cstddef>       // for std::size_t, etc.
#include <cstdint>       // for std::uint8_t, etc.
#include <cstdlib>       // for std::malloc, etc.
#include <cstring>       // for std::strlen, std::memcpy, etc.
#include <memory>        // for std::unique_ptr<T>
#include <string>        // for std::string
#include <system_error>  // for std::error_code
#include <type_traits>   // for std::make_unsigned<T>, etc.

#include "llvm/ADT/StringRef.h"        // for llvm::StringRef
#include "llvm/Support/raw_ostream.h"  // for llvm::raw_ostream
#include "ofp/config.h"

// Require C++11 -- std::string storage is guaranteed contiguous.
#if !defined(_MSC_VER)
static_assert(__cplusplus >= 201103L, "C++11 required");
#else
// Ignore __cplusplus value for MS compiler.
static_assert(_MSC_VER >= 1910, "VS 2017 required");
#endif
static_assert(std::is_same<std::uint8_t, unsigned char>::value,
              "Expected std::uint8_t to be implemented using unsigned char");

#if defined(__clang__)
#define OFP_BEGIN_IGNORE_PADDING   \
  _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Wpadded\"")
#define OFP_END_IGNORE_PADDING _Pragma("clang diagnostic pop")
#define OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR \
  _Pragma("clang diagnostic push")          \
      _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"")
#define OFP_END_IGNORE_GLOBAL_CONSTRUCTOR _Pragma("clang diagnostic pop")
#define OFP_BEGIN_IGNORE_USED_BUT_UNUSED \
  _Pragma("clang diagnostic push")       \
      _Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"")
#define OFP_END_IGNORE_USED_BUT_UNUSED _Pragma("clang diagnostic pop")
#define OFP_BEGIN_EXIT_TIME_DESTRUCTORS \
  _Pragma("clang diagnostic push")      \
      _Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"")
#define OFP_END_EXIT_TIME_DESTRUCTORS _Pragma("clang diagnostic pop")
#else
#define OFP_BEGIN_IGNORE_PADDING
#define OFP_END_IGNORE_PADDING
#define OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR
#define OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
#define OFP_BEGIN_IGNORE_USED_BUT_UNUSED
#define OFP_END_IGNORE_USED_BUT_UNUSED
#define OFP_BEGIN_EXIT_TIME_DESTRUCTORS
#define OFP_END_EXIT_TIME_DESTRUCTORS
#endif

#if defined(__clang__) || defined(_MSC_VER)
#define OFP_ALIGNAS(x) alignas(x)
#else
#define OFP_ALIGNAS(x) __attribute__((aligned(x)))
#endif

#define OFP_DEPRECATED __attribute__((deprecated))

namespace ofp {

// Metaprogramming Utilities

template <bool B, class T>
using EnableIf = typename std::enable_if<B, T>::type;

template <bool B, class T, class F>
using Conditional = typename std::conditional<B, T, F>::type;

template <class T>
using MakeSigned = typename std::make_signed<T>::type;

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
using SignedInt32 = std::int32_t;

using Milliseconds = std::chrono::milliseconds;
using TimeClock = std::chrono::steady_clock;
using TimePoint = TimeClock::time_point;

// Literal type `ms` is not supported until C++14. Use _ms for now.

constexpr Milliseconds operator"" _ms(unsigned long long int msec) {  // NOLINT
  return Milliseconds{msec};
}

template <class T>
constexpr MakeSigned<T> Signed_cast(T value) {
  return static_cast<MakeSigned<T>>(value);
}

template <class T>
constexpr MakeUnsigned<T> Unsigned_cast(T value) {
  return static_cast<MakeUnsigned<T>>(value);
}

template <class T>
constexpr UInt8 UInt8_cast(T value) {
  static_assert(sizeof(T) == sizeof(UInt8), "Use UInt8_narrow_cast().");
  return static_cast<UInt8>(Unsigned_cast(value));
}

template <class T>
constexpr UInt16 UInt16_cast(T value) {
  static_assert(sizeof(T) <= sizeof(UInt16), "Use UInt16_narrow_cast().");
  return static_cast<UInt16>(Unsigned_cast(value));
}

template <class T>
constexpr UInt32 UInt32_cast(T value) {
  static_assert(sizeof(T) <= sizeof(UInt32), "Use UInt32_narrow_cast().");
  return static_cast<UInt32>(Unsigned_cast(value));
}

template <class T>
constexpr UInt64 UInt64_cast(T value) {
  static_assert(sizeof(T) <= sizeof(UInt64), "No UInt64 narrow casts.");
  return static_cast<UInt64>(Unsigned_cast(value));
}

template <class T>
constexpr UInt8 UInt8_narrow_cast(T value) {
  static_assert(sizeof(T) > sizeof(UInt8), "Use UInt8_cast().");
  return static_cast<UInt8>(value);
}

template <class T>
constexpr UInt16 UInt16_narrow_cast(T value) {
  static_assert(sizeof(T) > sizeof(UInt16), "Use UInt16_cast().");
  return static_cast<UInt16>(value);
}

template <class T>
constexpr UInt32 UInt32_narrow_cast(T value) {
  // Support case where T == size_t on 32-bit system
  static_assert(sizeof(T) > sizeof(UInt32) || std::is_same<T, size_t>::value,
                "Use UInt32_cast().");
  return static_cast<UInt32>(value);
}

inline UInt16 UInt16_unaligned(const void *ptr) {
  UInt16 val;
  std::memcpy(&val, ptr, sizeof(val));
  return val;
}

inline UInt32 UInt32_unaligned(const void *ptr) {
  UInt32 val;
  std::memcpy(&val, ptr, sizeof(val));
  return val;
}

/// \returns true if type is a literal type.
template <class T>
constexpr bool IsLiteralType() {
  return std::is_literal_type<T>::value;
}

/// \returns true if type has standard layout.
template <class T>
constexpr bool IsStandardLayout() {
  return std::is_standard_layout<T>::value;
}

/// \returns true if type is trivially copyable.
template <class T>
constexpr bool IsTriviallyCopyable() {
#if defined(__clang__) || defined(_MSC_VER)
  return std::is_trivially_copyable<T>::value;
#else
  // GCC 4.7.2 doesn't define std::is_trivially_copyable. We only use this
  // macro in static_asserts, so the easiest fix is to always return true.
  return true;
#endif
}

/// \returns true if type `From` can be implicitly converted to type `To`.
template <class From, class To>
constexpr bool IsConvertible() {
#if !defined(_MSC_VER)
  return std::is_convertible<From, To>::value;
#else
  // std::is_convertible doesn't seem to work in VS 2017. We only use this
  // macro in static_asserts, so the easiest fix is to always return true.
  return true;
#endif
}

/// \returns number of elements in array.
template <class T, size_t N>
constexpr size_t ArrayLength(T (&)[N]) {
  return N;
}

/// \returns byte pointer for any memory address.
constexpr const UInt8 *BytePtr(const void *data) {
  return static_cast<const UInt8 *>(data);
}

/// \returns mutable byte pointer for any mutable memory address.
constexpr UInt8 *MutableBytePtr(void *data) {
  return static_cast<UInt8 *>(data);
}

/// Removes a `const` qualifier from a reference. This is necessary in rare
/// cases when interacting with important external API's that are less
/// `const-strict`. No other instances of const_cast should appear in the code.
template <class T>
T &RemoveConst_cast(const T &v) {
  return const_cast<T &>(v);
}

template <class T>
T *RemoveConst_cast(const T *v) {
  return const_cast<T *>(v);
}

/// Convert raw buffer to a hexadecimal string (upper case). The resulting
/// string contains only hexadecimal characters, no delimiters.
///
/// \param  data pointer to input buffer
/// \param  length size of input buffer
/// \return hexadecimal string
std::string RawDataToHex(const void *data, size_t length);

/// Convert raw buffer to a hexadecimal string (upper case). The resulting
/// string contains only hexadecimal characters, no delimiters.
///
/// \param  data pointer to input buffer
/// \param  length size of input buffer
/// \param  os output stream
void RawDataToHex(const void *data, size_t length, llvm::raw_ostream &os);

/// Convert raw buffer to a hexadecimal string (upper case).
///
/// The resulting string is formatted according to `delimiter` and `word`. The
/// delimiter specifies a character to insert between each run of hexadecimal
/// chars. `word` specifies the number of bytes between delimiters.
///
/// For example, delimiter=':' and word=2 produces "0001:0203:0405"
///
/// \param  data pointer to raw memory
/// \param  length size of input buffer
/// \param  delimiter character to insert to format the hex string
/// \param  word number of bytes between delimiters
/// \return hexadecimal string
std::string RawDataToHex(const void *data, size_t length, char delimiter,
                         int word = 1);

/// Convert a hexadecimal string to raw memory. Only write up to `length` bytes.
///
/// Ignore non-hex digits and the odd final hex digit. If there are fewer than
/// `length` bytes converted from the hex string, set the remaining bytes to
/// zero. If `error` is not null, set value to true when there are non-hex
/// digits or an odd number of hex digits.
///
/// \param  hex string containing hexadecimal characters
/// \param  data output buffer
/// \param  length size of output buffer
/// \param  error ptr to optional boolean error result
/// \return number of bytes resulting from hexadecimal string
size_t HexToRawData(const std::string &hex, void *data, size_t length,
                    bool *error = nullptr);

/// Convert a (small) fixed size array to hexadecimal using lower case and ':'
/// as the delimiter. Defined for Length=6 and Length=8. Returns buf ptr.
template <size_t Length>
char *RawDataToHexDelimitedLowercase(const std::array<UInt8, Length> &data,
                                     char (&buf)[Length * 3]);

extern template char *RawDataToHexDelimitedLowercase(
    const std::array<UInt8, 8> &data, char (&buf)[24]);

extern template char *RawDataToHexDelimitedLowercase(
    const std::array<UInt8, 6> &data, char (&buf)[18]);

/// Convert a hexadecimal string with each hex-pair delimited by ':'.
///
/// There must be one or two hex digits between each ':'. A single
/// hex digit 'h' is treated as if prefixed by 0, e.g. '0h'. Anything
/// other than a hex digit or ':' is treated as an error.
///
/// \param  s string containing 'hh:hh:hh:...:hh'
/// \param  data output buffer
/// \param  length size of output buffer
/// \return size of output or 0 if there is an error
size_t HexDelimitedToRawData(llvm::StringRef s, void *data, size_t length,
                             char delimiter = ':');

/// Convert a hexadecimal string with no delimiters or white space.
///
/// \param  s string containing 'hhhhhhhhhh'
/// \param  data output buffer
/// \param  length size of output buffer
/// \return size of output or 0 if there is an error
size_t HexStrictToRawData(llvm::StringRef s, void *data, size_t length);

/// Convert a hexadecimal string to raw memory. Ignore non-hex digits and the
/// odd final hex digit.
///
/// \param  hex string containing hexadecimal characters
/// \return string containing raw bytes
std::string HexToRawData(const std::string &hex);

/// Convert raw buffer to a base64.
///
/// \param  data pointer to input buffer
/// \param  length size of input buffer
/// \return base64 string
std::string RawDataToBase64(const void *data, size_t length);

/// Return true if memory block is filled with given byte value.
///
/// \param  data   pointer to memory block
/// \param  length length of memory block
/// \param  fill   fill byte
/// \return true if memory block is filled with given byte value.
bool IsMemFilled(const void *data, size_t length, char fill);

/// Copy data to destination and mask it with the given data mask.
///
/// \param  dest  pointer to destination
/// \param  data  pointer to source
/// \param  mask  pointer to mask bytes
/// \param  length size of source and mask buffers
void MemCopyMasked(void *dest, const void *data, const void *mask,
                   size_t length);

/// Return true if pointer is aligned to specified byte boundary.
///
/// For example, to check if pointer is 64-bit aligned:
///    IsPtrAligned(ptr, 8)
///
/// \param  ptr  pointer to memory
/// \param  byteBoundary alignment to 2, 4, or 8 bytes
/// \return true if pointer is aligned.
inline bool IsPtrAligned(const void *ptr, size_t byteBoundary) {
  return (reinterpret_cast<uintptr_t>(ptr) & (byteBoundary - 1)) == 0;
}

/// Set a watchdog alarm to detect infinite loops.
///
/// Calling SetWatchdogTimer(secs) when there is already an alarm pushes back
/// the alarm.
///
/// To clear the alarm, call SetWatchdogTimer(0).
///
/// \param secs  number of seconds until alarm
void SetWatchdogTimer(unsigned secs);

/// Return a constructed unique_ptr for the specified type by forwarding
/// arguments to the constructor.
///
/// \return unique ptr to new object.
template <class T, class... Args>
std::unique_ptr<T> MakeUniquePtr(Args &&...args) {
  static_assert(!std::is_array<T>::value, "Only supports non-array types.");
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class NonCopyable {
 protected:
  NonCopyable() = default;
  ~NonCopyable() = default;

 private:
  NonCopyable(const NonCopyable &) = default;
  NonCopyable &operator=(const NonCopyable &) = default;
};

static_assert(IsTriviallyCopyable<NonCopyable>(), "Expected trivial copyable.");

/// Cast a const pointer to another type, asserting that the ptr has the
/// expected alignment in debug build.
template <class T>
#ifdef NDEBUG
constexpr const T *Interpret_cast(const void *ptr) {
#else
inline const T *Interpret_cast(const void *ptr) {
  assert(IsPtrAligned(ptr, alignof(T)) && "ptr has unexpected alignment");
#endif  // NDEBUG
  return reinterpret_cast<const T *>(ptr);
}

namespace detail {

// Template function that can be used to implement `toString()` function in
// types that can output to a raw_ostream.
template <class Type>
std::string ToString(const Type &value) {
  std::string result;
  llvm::raw_string_ostream oss{result};
  oss << value;
  return oss.str();
}

}  // namespace detail
}  // namespace ofp

// Place forward declarations of YAML classes here.

namespace llvm {
namespace yaml {

template <class T>
struct MappingTraits;

template <class T>
struct ScalarTraits;

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_TYPES_H_
