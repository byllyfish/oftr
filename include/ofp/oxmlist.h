//  ===== ---- ofp/oxmlist.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the OXMList class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXMLIST_H
#define OFP_OXMLIST_H

#include "ofp/oxmrange.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

// OXMList is an ordered sequence of values stored (without padding) in a
// memory buffer.
// It allows multiple values for the same key (used for prereqs).

class OXMList {
public:
	OXMList() = default;
	OXMList(const OXMRange &range);

	OXMIterator begin() const { return OXMIterator(buf_.begin()); }
	OXMIterator end() const { return OXMIterator(buf_.end()); }

	const UInt8 *data() const { return buf_.begin(); }
	size_t size() const { return buf_.size(); }
	
	OXMRange toRange() const { return OXMRange{buf_.data(), buf_.size()}; }

	template <class ValueType>
	void add(ValueType value);
	
	template <class ValueType>
	void add(ValueType value, ValueType mask);
	
	void add(OXMIterator iter);
	void addSignal(OXMType signal);
	void insertSignal(OXMIterator pos, OXMType signal);
	void pad8(unsigned offset);
	
	void clear() {
		buf_.clear();
	}

	OXMIterator replace(OXMIterator pos, OXMIterator end, OXMType type, const void *data, size_t len);
	
	bool operator==(const OXMList &rhs) const { return buf_ == rhs.buf_; }
	bool operator!=(const OXMList &rhs) const { return !(*this == rhs); }

private:
	ByteList buf_;
	
	//void add(const void *data, size_t len);
	void insert(OXMIterator pos, const void *data, size_t len);

	void add(OXMType type, const void *data, size_t len);
	void add(OXMType type, const void *data, const void *mask, size_t len);
};

template <class ValueType>
inline
void OXMList::add(ValueType value)
{
	static_assert(sizeof(value) < 256, "oxm_length must be <= 255.");
	
	add(ValueType::type(), &value, sizeof(value));
}

template <class ValueType>
inline
void OXMList::add(ValueType value, ValueType mask)
{
	static_assert(sizeof(value) < 128, "oxm_length must be <= 255.");
	
	add(ValueType::type().withMask(), &value, &mask, sizeof(value));
}


inline void OXMList::add(OXMIterator iter)
{
	buf_.add(iter.data(), iter.size());
}

inline void OXMList::addSignal(OXMType signal)
{
	buf_.add(&signal, sizeof(signal));
}

inline void OXMList::insertSignal(OXMIterator pos, OXMType signal)
{
	insert(pos, &signal, sizeof(signal));
}

inline void OXMList::pad8(unsigned n)
{
	size_t len = PadLength(n + size()) - (n + size());
	if (len > 0) {
		assert(len < 8);
		Padding<8> padding;
		buf_.add(&padding, len);
	}
}

} // </namespace ofp>

#endif // OFP_OXMLIST_H
