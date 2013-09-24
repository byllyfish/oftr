//  ===== ---- ofp/padding.h -------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Provides utilties for padding structures to 8 byte boundaries.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PADDING_H
#define OFP_PADDING_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>


template <size_t N>
struct Padding {
	constexpr Padding() {}
	UInt8 pad_[N] = {};
};


constexpr size_t PadLength(size_t length)
{
	return 8 * ((length + 7) / 8);
}


namespace detail { // <namespace detail>

template <class ContentType>
class PaddedWithPadding {
public:
	PaddedWithPadding(const ContentType &content) : content_{content} {}
	operator const ContentType&() const { return content_; }
	operator ContentType&() { return content_; }

	void operator=(const ContentType &content) { content_ = content; }

	const ContentType &content() const { return content_; }

private:
	ContentType content_;
	Padding<8 - sizeof(ContentType) % 8> pad;
};

template <class ContentType>
class PaddedNoPadding {
public:
	PaddedNoPadding(const ContentType &content) : content_{content} {}
	operator const ContentType&() const { return content_; }
	operator ContentType&() { return content_; }

	void operator=(const ContentType &content) { content_ = content; }

	const ContentType &content() const { return content_; }

private:
	ContentType content_;
};

} // </namespace detail>


template <class ContentType>
using Padded = Conditional<sizeof(ContentType) % 8 != 0, 
							detail::PaddedWithPadding<ContentType>,
							detail::PaddedNoPadding<ContentType>>;


} // </namespace ofp>

#endif // OFP_PADDING_H
