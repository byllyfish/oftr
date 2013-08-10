#ifndef OFP_INTERNALCHANNEL_IMPL_H
#define OFP_INTERNALCHANNEL_IMPL_H

#include "ofp/channel.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

/**
 *  InternalChannel is an interface for a channel that can receive messages
 *  posted from other InternalChannels. This interface also supports binding
 *  auxillary connections to their main connection, and a main connection to a
 *  a linked list of auxiliary connections.
 */
class InternalChannel : public Channel {
public:
	virtual void postMessage(InternalChannel *source, Message *message) = 0;

	virtual UInt8 auxiliaryID() const = 0;

	virtual InternalChannel *mainConnection() = 0;
	virtual void setMainConnection(InternalChannel *channel) = 0;

	virtual InternalChannel *nextAuxiliaryConnection() = 0;
	virtual void setNextAuxiliaryConnection(InternalChannel *channel) = 0;
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_INTERNALCHANNEL_IMPL_H
