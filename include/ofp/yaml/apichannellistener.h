#ifndef OFP_YAML_APICHANNELLISTENER_H
#define OFP_YAML_APICHANNELLISTENER_H

#include "ofp/channellistener.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class ApiServer;

class ApiChannelListener : public ChannelListener {
public:
	ApiChannelListener(ApiServer *server): server_{server} {}

	void onChannelUp(Channel *channel) override;
    void onChannelDown(Channel *channel) override;

    void onMessage(const Message *message) override;
    void onException(const Exception *exception) override;

    void onTimer(UInt32 timerID) override;

private:
	ApiServer *server_;
	Channel *channel_;
};

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_APICHANNELLISTENER_H
