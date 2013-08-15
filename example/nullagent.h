#ifndef NULLAGENT_H
#define NULLAGENT_H

#include "ofp.h"

using ofp::UInt16;
using ofp::Message;
using ofp::Channel;


class NullAgent : public ofp::AbstractChannelListener {
public:

    static NullAgent *Factory() { return new NullAgent; }

    void onChannelUp(Channel *channel) override
    {
        ofp::log::debug("NullAgent channel up.");
    }

    void onChannelDown(Channel *channel) override
    {
        ofp::log::debug("NullAgent channel down.");
    }

    void onMessage(const Message *message) override;

private:
    void onFeaturesRequest(const Message *message);
    void onFlowMod(const Message *message);
    void sendError(const Message *message, UInt16 type, UInt16 code);

    static ofp::Features gFeatures;
};

void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    case ofp::FeaturesRequest::Type:
        onFeaturesRequest(message);
        break;

    default:
        sendError(message, 1, 1);
        break;
    }
}

void NullAgent::onFeaturesRequest(const Message *message)
{
    ofp::FeaturesReplyBuilder msg{message};
    msg.setFeatures(gFeatures);
    msg.send(message->source());
}


void NullAgent::onFlowMod(const Message *message)
{
    auto flowMod = ofp::FlowMod::cast(message);
    if (!flowMod) {
        sendError(message, ofp::OFPET_BAD_REQUEST, ofp::OFPBRC_BAD_TYPE);
        return;
    }

    
}

void NullAgent::sendError(const Message *message, UInt16 type, UInt16 code)
{
    ofp::ErrorBuilder msg{type, code, message};
    msg.send(message->source());
}

#endif // NULLAGENT_H
