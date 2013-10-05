#include "ofp/bucketlist.h"

using namespace ofp;


void BucketList::add(const Bucket &bucket)
{
	ActionRange actions = bucket.actions();
	
	assert(bucket.len_ == Bucket::SizeWithoutActionRange + actions.size());

	buf_.add(&bucket, Bucket::SizeWithoutActionRange);
	buf_.add(actions.data(), actions.size());
}