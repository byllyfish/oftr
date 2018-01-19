// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PADDING_H_
#define OFP_PADDING_H_

#include "ofp/types.h"

namespace ofp {

template <size_t N>
struct Padding {
  constexpr Padding() {}
  UInt8 pad_[N] = {};
};

constexpr size_t PadLength(size_t length) {
  return 8 * ((length + 7) / 8);
}

namespace detail {

template <class ContentType>
class PaddedWithPadding {
 public:
  explicit PaddedWithPadding(const ContentType &content) : content_{content} {}
  operator const ContentType &() const { return content_; }
  operator ContentType &() { return content_; }

  void operator=(const ContentType &content) { content_ = content; }

  const ContentType &content() const { return content_; }

 private:
  ContentType content_;
  Padding<8 - sizeof(ContentType) % 8> pad;
};

template <class ContentType>
class PaddedNoPadding {
 public:
  explicit PaddedNoPadding(const ContentType &content) : content_{content} {}
  operator const ContentType &() const { return content_; }
  operator ContentType &() { return content_; }

  void operator=(const ContentType &content) { content_ = content; }

  const ContentType &content() const { return content_; }

 private:
  ContentType content_;
};

}  // namespace detail

template <class ContentType>
using Padded = Conditional<sizeof(ContentType) % 8 != 0,
                           detail::PaddedWithPadding<ContentType>,
                           detail::PaddedNoPadding<ContentType>>;

}  // namespace ofp

#endif  // OFP_PADDING_H_
