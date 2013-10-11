#include "ofp/bucket.h"

using namespace ofp;


ActionRange Bucket::actions() const
{
	assert(len_ >= sizeof(Bucket));

	size_t actLen = len_ - sizeof(Bucket);
	return ActionRange{ByteRange{BytePtr(this) + sizeof(Bucket), actLen}};
}


void BucketBuilder::setActions(ActionRange actions)
{
	bkt_.len_ = UInt16_narrow_cast(sizeof(Bucket) + actions.size());
	actions_ = actions;
}