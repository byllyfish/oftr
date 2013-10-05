#ifndef OFP_BUCKETRANGE_H
#define OFP_BUCKETRANGE_H

#include "ofp/byterange.h"
#include "ofp/bucketiterator.h"

namespace ofp { // <namespace ofp>

class BucketRange {
public:
	BucketRange() = default;

	BucketIterator begin() const;
	BucketIterator end() const;

	size_t itemCount() const;

private:
	ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_BUCKETRANGE_H
