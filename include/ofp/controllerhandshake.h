#ifndef OFP_CONTROLLERHANDSHAKE_H
#define OFP_CONTROLLERHANDSHAKE_H

#include "ofp/channellistener.h"
#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>

class ProtocolVersions;
class InternalChannel;


class ControllerHandshake : public ChannelListener {
public:

	ControllerHandshake(InternalChannel *channel, ProtocolVersions versions, Factory listenerFactory);

	void onChannelUp(Channel *channel) override;
	void onChannelDown() override;
	void onMessage(const Message *message) override;
	void onException(const Exception *exception) override;

private:
	InternalChannel *channel_;
	ProtocolVersions versions_;
	Factory listenerFactory_;

	void onHello(const Message *message);
	void onFeaturesReply(const Message *message);
	void onError(const Message *message);

};

} // </namespace ofp>

#endif // OFP_CONTROLLERHANDSHAKE_H
