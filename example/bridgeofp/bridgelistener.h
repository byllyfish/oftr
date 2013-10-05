#ifndef BRIDGELISTENER_H
#define BRIDGELISTENER_H

#include "ofp.h"

namespace bridge { // <namespace bridge>

using namespace ofp;

class BridgeListener : public ChannelListener {
public:
	explicit BridgeListener(BridgeListener *otherBridge);
	explicit BridgeListener(const IPv6Address &remoteAddr);
	~BridgeListener();

    void onChannelUp(Channel *channel) override;
    //void onChannelDown(Channel *channel) override;

    void onMessage(const Message *message) override;
    //void onException(const Exception *exception) override;

    Channel *channel() const { return channel_; }

private:
	Channel *channel_ = nullptr;
	IPv6Address remoteAddr_;
	BridgeListener *otherBridge_ = nullptr;

	void forget(BridgeListener *other);

	static void translateAndForward(const Message *message, Channel *channel);
};

} // </namespace bridge>

#endif // BRIDGELISTENER_H
