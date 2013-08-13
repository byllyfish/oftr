#ifndef OFP_DEFAULTHANDSHAKE_H
#define OFP_DEFAULTHANDSHAKE_H

#include "ofp/channellistener.h"
#include "ofp/protocolversions.h"
#include "ofp/driver.h"

namespace ofp { // <namespace ofp>

class InternalChannel;

OFP_BEGIN_IGNORE_PADDING
class DefaultHandshake : public ChannelListener {
public:

	DefaultHandshake(InternalChannel *channel, Driver::Role role, ProtocolVersions versions, Factory listenerFactory);

	void onChannelUp(Channel *channel) override;
	void onChannelDown(Channel *channel) override;
	void onMessage(const Message *message) override;
	void onException(const Exception *exception) override;

private:
	InternalChannel *channel_;
	ProtocolVersions versions_;
	Factory listenerFactory_;
	Driver::Role role_;
	
	void onHello(const Message *message);
	void onFeaturesReply(const Message *message);
	void onError(const Message *message);

	void installNewChannelListener();
};
OFP_END_IGNORE_PADDING

} // </namespace ofp>

#endif // OFP_DEFAULTHANDSHAKE_H
