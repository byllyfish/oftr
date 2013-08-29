/// \file ofp.h
/// \brief Top-level header for the ofp library.

#ifndef OFP_OFP_H
#define OFP_OFP_H

#include "ofp/driver.h"
#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/message.h"
#include "ofp/features.h"
#include "ofp/headeronly.h"
#include "ofp/featuresreply.h"
#include "ofp/flowmod.h"
#include "ofp/packetin.h"
#include "ofp/flowremoved.h"
#include "ofp/portstatus.h"
#include "ofp/getasyncreply.h"
#include "ofp/getconfigreply.h"
#include "ofp/error.h"
#include "ofp/exception.h"

namespace ofp { // <namespace ofp>

/// \brief Listens for incoming OpenFlow connections on the default port.
/// When a switch connects, uses listenerFactory to create a ChannelListener.
/// Once the driver begins listening for incoming connections, it will run
/// forever. (TODO: provide a way to shutdown the driver). If the driver fails
/// to listen on the default port, this function returns an error.
///
/// \param listenerFactory no-arg function that creates a ChannelListener
/// \param versions set of protocol versions to accept
/// \returns error result
Exception runController(ChannelListener::Factory listenerFactory,
                        ProtocolVersions versions = ProtocolVersions::All);

/// \brief Connects to an OpenFlow controller at the specified address on the 
/// default port. 
/// After connecting, uses listenerFactory to create a ChannelListener.
/// 
/// \param  features 		Agent information including datapath ID.
/// \param  remoteAddress   IPv6 (possibly IPv4-mapped) address of controller
/// \param  listenerFactory no-arg function that create a ChannelListener
/// \param  versions set of protocol versions to accept
/// \return error result
Exception runAgent(const Features &features, const IPv6Address &remoteAddress,
                   ChannelListener::Factory listenerFactory,
                   ProtocolVersions versions = ProtocolVersions::All);

} // </namespace ofp>

#endif // OFP_OFP_H
