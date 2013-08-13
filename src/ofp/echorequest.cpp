#include "ofp/echorequest.h"
#include "ofp/message.h"


const ofp::EchoRequest *ofp::EchoRequest::cast(const Message *message)
{
	assert(message->type() == OFPT_ECHO_REQUEST);

	const EchoRequest *msg = reinterpret_cast<const EchoRequest *>(message->data());
    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}



bool ofp::EchoRequest::validateLength(size_t length) const
{
	return (header_.length() == length);
}
