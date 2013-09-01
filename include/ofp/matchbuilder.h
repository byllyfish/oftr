//  ===== ---- ofp/matchbuilder.h --------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the MatchBuilder class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MATCHBUILDER_H
#define OFP_MATCHBUILDER_H

#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/prerequisites.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class MatchBuilder {
public:

	MatchBuilder() = default;

	const UInt8 *data() const { return list_.data(); }
	size_t size() const { return list_.size(); }

	/// \returns number of items in the match.
    size_t itemCount() const
    {
        return OXMIterator::distance(list_.begin(), list_.end());
    }

	OXMIterator begin() const
    {
        return list_.begin();
    }
    
    OXMIterator end() const
    {
        return list_.end();
    }

	template <class ValueType>
	void add(ValueType value) {
		Prerequisites::insertAll(&list_, ValueType::prerequisites());
		if (!Prerequisites::substitute(&list_, ValueType::type(), &value, sizeof(value))) {
			list_.add(value);
		}
	}
	
	template <class ValueType>
	void add(ValueType value, ValueType mask) {
		assert(ValueType::maskSupported());
		Prerequisites::insertAll(&list_, ValueType::prerequisites());
		if (!Prerequisites::substitute(&list_, ValueType::type(), &value, &mask, sizeof(value))) {
			list_.add(value, mask);
		}
	}

	bool validate() const {
		return Prerequisites::checkAll(list_.toRange()) &&
				!Prerequisites::duplicateFieldsDetected(list_.toRange());
	}

	OXMRange toRange() const { return list_.toRange(); }

private:
	OXMList list_;
};

} // </namespace ofp>

#endif // OFP_MATCHBUILDER_H
