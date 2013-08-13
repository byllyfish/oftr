#ifndef OFP_OFP_H
#define OFP_OFP_H

#include "ofp/driver.h"
#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/message.h"
#include "ofp/features.h"
#include "ofp/featuresrequest.h"
#include "ofp/featuresreply.h"
#include "ofp/exception.h"

namespace ofp { // <namespace ofp>

void runDefaultController(ChannelListener::Factory listenerFactory);
void runDefaultAgent(const IPv6Address &remoteAddress, UInt16 remotePort,
                     ChannelListener::Factory listenerFactory);

} // </namespace ofp>

#endif // OFP_OFP_H
