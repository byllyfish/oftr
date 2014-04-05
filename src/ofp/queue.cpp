#include "ofp/queue.h"

using namespace ofp;


PropertyRange Queue::properties() const
{
    return ByteRange{BytePtr(this) + sizeof(Queue), len_ - sizeof(Queue)};
}


void QueueBuilder::setProperties(const PropertyRange &properties)
{
    properties_.assign(properties);
    queue_.len_ = UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
}
