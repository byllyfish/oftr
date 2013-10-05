#include "ofp/flowremoved.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;


const FlowRemoved *FlowRemoved::cast(const Message *message)
{
	return message->cast<FlowRemoved>();
}


bool FlowRemoved::validateLength(size_t length) const
{
	// FIXME
	return false;
}

FlowRemovedBuilder::FlowRemovedBuilder(const FlowRemoved *msg) : msg_{*msg}
{
	// FIXME copy match...
}


UInt32 FlowRemovedBuilder::send(Writable *channel)
{
	// FIXME
	return 0;
}
 