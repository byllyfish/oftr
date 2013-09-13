//  ===== ---- ofp/actionrange.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the ActionRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONRANGE_H
#define OFP_ACTIONRANGE_H

#include "ofp/byterange.h"
#include "ofp/writable.h"
#include "ofp/actioniterator.h"

namespace ofp { // <namespace ofp>

class ActionRange {
public:
	ActionRange() = default;
	ActionRange(const ByteRange &range) : range_{range} {}

	size_t itemCount() const { return ActionIterator::distance(begin(), end()); }
	
	ActionIterator begin() const { return ActionIterator{data()}; }
	ActionIterator end() const { return ActionIterator{data() + size()}; }

	const UInt8 *data() const { return range_.data(); }
	size_t size() const { return range_.size(); }
	
	/// \returns Size of action list when written to channel using the specified
	/// protocol version.
	size_t writeSize(Writable *channel);

	/// \brief Writes action list to the channel using the specified protocol
	/// version.
	void write(Writable *channel);

private:
	ByteRange range_;

	static unsigned writeSizeMinusSetFieldV1(ActionIterator iter);
	static void writeSetFieldV1(ActionIterator iter, Writable *channel);
};

} // </namespace ofp>

#endif // OFP_ACTIONRANGE_H
