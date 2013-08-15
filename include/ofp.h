#ifndef OFP_OFP_H
#define OFP_OFP_H

#include "ofp/driver.h"
#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/message.h"
#include "ofp/features.h"
#include "ofp/featuresrequest.h"
#include "ofp/featuresreply.h"
#include "ofp/flowmod.h"
#include "ofp/error.h"
#include "ofp/exception.h"

namespace ofp { // <namespace ofp>

/**
 *  Listen for OpenFlow connections from OpenFlow switches on the default port.
 *  When a switch connects, use listenerFactory to create a ChannelListener.
 *  Once the driver begins listening for incoming connections, it will run
 *  forever. (TODO: provide a way to shutdown the driver). If the driver fails
 *  to listen on the default port, this function will return an error.
 *
 *  @param  listenerFactory no-arg function that creates a ChannelListener
 *  @return error result
 */
Exception runDefaultController(ChannelListener::Factory listenerFactory);

/**
 *  Connect to an OpenFlow controller at the specified address on the default
 *  port. After connecting, use listenerFactory to create a ChannelListener.
 *  (TODO: retry the connection)
 *
 *  @param  remoteAddress   IPv6 (possibly IPv4-mapped) address
 *  @param  listenerFactory no-arg function that create a ChannelListener
 *  @return error result
 */
Exception runDefaultAgent(const IPv6Address &remoteAddress,
                          ChannelListener::Factory listenerFactory);

} // </namespace ofp>

#endif // OFP_OFP_H
