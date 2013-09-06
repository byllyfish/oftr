//  ===== ---- ofp/smallcstring.h --------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the SmallCString class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SMALLCSTRING_H
#define OFP_SMALLCSTRING_H

#include <array>
#include <algorithm>

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

/// \returns smaller of string length or maxlen.
size_t strlen(const char *s, size_t maxlen);

} // </namespace detail>

/// \brief Implements a fixed size, null-terminated C string.
/// \remarks Binary representation is standard layout.
template <size_t Size>
class SmallCString {
public:
    using ArrayType = std::array<char, Size>;

    constexpr SmallCString() : str_{} {}

    SmallCString(const SmallCString &rhs) : str_(rhs.str_)
    {
        str_.back() = 0;
    }

    SmallCString(const char *cstr) {
        operator=(cstr);
    }

    constexpr size_t capacity() const
    {
        return Size - 1;
    }

    bool empty() const
    {
        return str_.front() == 0;
    }

    size_t length() const
    {
        return detail::strlen(c_str(), capacity());
    }

    const char *c_str() const
    {
        return str_.data();
    }

    const ArrayType &toArray() const {
        return str_;
    }

    void operator=(const SmallCString &rhs)
    {
        str_ = rhs.str_;
    }

    void operator=(const char *cstr);

private:
    ArrayType str_;
};

template <size_t Size>
bool operator==(const SmallCString<Size> &lhs, const SmallCString<Size> &rhs);

template <size_t Size>
inline bool operator==(const SmallCString<Size> &lhs, const SmallCString<Size> &rhs)

{
    return lhs.toArray() == rhs.toArray();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //

namespace detail { // <namespace detail>

inline size_t strlen(const char *s, size_t maxlen)
{
    auto p = std::find(s, s + maxlen, '\0');
    return Unsigned_cast(p - s);
}

} // </namespace detail>

#if 0
/// Copies C string and sets the remaining bytes to zero. The last byte will 
/// always be zero.
template <size_t Size>
inline SmallCString<Size>::SmallCString(const char *cstr)
{
	size_t len = detail::strlen(cstr, capacity());
	assert(len <= capacity());
    std::memcpy(&str_, cstr, len);
    std::memset(&str_[len], 0, capacity() - len);
    str_.back() = 0;
}
#endif

/// Copies C string and sets the remaining bytes to zero. The last byte will 
/// always be zero.
template <size_t Size>
inline void SmallCString<Size>::operator=(const char *cstr)
{
    size_t len = detail::strlen(cstr, capacity());
    assert(len <= capacity());
    std::memcpy(&str_, cstr, len);
    std::memset(&str_[len], 0, capacity() - len);
    str_.back() = 0;
}

} // </namespace ofp>

#endif // OFP_SMALLCSTRING_H
