#ifndef OFP_DEFAULTHANDSHAKE_H
#define OFP_DEFAULTHANDSHAKE_H

#include "ofp/channellistener.h"
#include "ofp/protocolversions.h"
#include "ofp/driver.h"

namespace ofp { // <namespace ofp>

class Connection;

OFP_BEGIN_IGNORE_PADDING
class DefaultHandshake : public ChannelListener {
public:

	DefaultHandshake(Connection *channel, Driver::Role role, ProtocolVersions versions, Factory listenerFactory);

	void onChannelUp(Channel *channel) override;
	void onChannelDown(Channel *channel) override;
	void onMessage(const Message *message) override;
	void onException(const Exception *exception) override;
	void onTimer(UInt32 timerID) override {}
	
	Driver::Role role() const { return role_; }
	
	void setStartingVersion(UInt8 version) { startingVersion_ = version; }
	void setStartingXid(UInt32 xid) { startingXid_ = xid; }
	void setConnection(Connection *channel) { channel_ = channel; }

private:
	Connection *channel_;
	ProtocolVersions versions_;
	Factory listenerFactory_;
	Driver::Role role_;
	UInt32 startingXid_ = 0;
	UInt8 startingVersion_ = 0;

	void onHello(const Message *message);
	void onFeaturesReply(const Message *message);
	void onError(const Message *message);

	void installNewChannelListener();
};
OFP_END_IGNORE_PADDING

} // </namespace ofp>

#endif // OFP_DEFAULTHANDSHAKE_H
