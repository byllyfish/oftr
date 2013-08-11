#include "ofp/error.h"
#include "ofp/log.h"


ofp::ErrorBuilder::ErrorBuilder(UInt16 type, UInt16 code)
{
	msg_.type_ = type;
	msg_.code_ = code;
}

void ofp::ErrorBuilder::send(Channel *channel)
{
	log::debug(__PRETTY_FUNCTION__);
}
