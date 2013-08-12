#include "ofp/message.h"
#include "ofp/internalchannel.h"


ofp::Channel *ofp::Message::source() const
{
    return channel_;
}