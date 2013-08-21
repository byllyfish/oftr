#include "ofp/message.h"
#include "ofp/connection.h"


ofp::Channel *ofp::Message::source() const
{
    return channel_;
}