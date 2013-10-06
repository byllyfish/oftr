#ifndef OFP_BUCKETRANGE_H
#define OFP_BUCKETRANGE_H

#include "ofp/byterange.h"
#include "ofp/bucketiterator.h"

namespace ofp { // <namespace ofp>

class BucketRange {
public:
	BucketRange() = default;
	explicit BucketRange(const ByteRange &range) : range_{range} {}

	size_t itemCount() const { return BucketIterator::distance(begin(), end()); }

	BucketIterator begin() const { return BucketIterator{range_.begin()}; }
	BucketIterator end() const { return BucketIterator{range_.end()}; }

private:
	ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_BUCKETRANGE_H
