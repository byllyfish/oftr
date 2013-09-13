#ifndef OFP_INSTRUCTIONRANGE_H
#define OFP_INSTRUCTIONRANGE_H

#include "ofp/byterange.h"
#include "ofp/instructioniterator.h"

namespace ofp { // <namespace ofp>

class InstructionRange {
public:

    InstructionRange(const ByteRange &range) : range_{range} {}
    
   /// \returns number of items in the range.
    size_t itemCount() const
    {
        return InstructionIterator::distance(begin(), end());
    }

    InstructionIterator begin() const
    {
        return InstructionIterator{range_.begin()};
    }

    InstructionIterator end() const
    {
        return InstructionIterator{range_.end()};
    }

    const UInt8 *data() const { return range_.data(); }
    size_t size() const { return range_.size(); }
    
private:
	ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_INSTRUCTIONRANGE_H
