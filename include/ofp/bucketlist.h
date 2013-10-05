#ifndef OFP_BUCKETLIST_H
#define OFP_BUCKETLIST_H

#include "ofp/bytelist.h"
#include "ofp/bucket.h"

namespace ofp { // <namespace ofp>


class BucketList {
public:
	BucketList() = default;

	const UInt8 *data() const { return buf_.data(); }
	size_t size() const { return buf_.size(); }
	
	void add(const Bucket &bucket);

private:
	ByteList buf_;
};

} // </namespace ofp>

#endif // OFP_BUCKETLIST_H
