//  ===== ---- ofp/ofp.h -----------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Top-level header for the ofp library..
//  ===== ------------------------------------------------------------ =====  //

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
#include "ofp/packetout.h"
#include "ofp/flowremoved.h"
#include "ofp/portstatus.h"
#include "ofp/getasyncreply.h"
#include "ofp/getconfigreply.h"
#include "ofp/error.h"
#include "ofp/setconfig.h"
#include "ofp/exception.h"
#include "ofp/actions.h"
#include "ofp/hello.h"
#include "ofp/experimenter.h"
#include "ofp/multipartrequest.h"
#include "ofp/multipartreply.h"
#include "ofp/rolerequest.h"
#include "ofp/rolereply.h"
#include "ofp/setasync.h"
#include "ofp/groupmod.h"
#include "ofp/portmod.h"
#include "ofp/tablemod.h"

namespace ofp { // <namespace ofp>

/// \brief Listens for incoming OpenFlow connections on the default port.
///
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
///
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
