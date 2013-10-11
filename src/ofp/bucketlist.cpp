#include "ofp/bucketlist.h"

using namespace ofp;


void BucketList::add(const BucketBuilder &bucket)
{
	ActionRange actions = bucket.actions();

	buf_.add(&bucket, BucketBuilder::SizeWithoutActionRange);
	buf_.add(actions.data(), actions.size());
}