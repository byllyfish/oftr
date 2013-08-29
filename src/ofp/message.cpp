#include "ofp/message.h"
#include "ofp/sys/connection.h"


ofp::Channel *ofp::Message::source() const
{
    return channel_;
}