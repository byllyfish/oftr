#ifndef NULLAGENT_H
#define NULLAGENT_H

#include "ofp.h"


static Features gFeatures{};

class NullAgent : public AbstractChannelListener {
public:

    static NullAgent *Factory() { return new NullAgent; }

    void onChannelUp(Channel *channel) override
    {
        log::debug(__PRETTY_FUNCTION__);
    }

    void onMessage(const Message *message) override;

private:
    void onFeaturesRequest(const Message *message);
    void onFlowMod(const Message *message);
    void sendError(const Message *message, UInt16 type, UInt16 code);
};

void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    case FeaturesRequest::Type:
        onFeaturesRequest(message);
        break;

    default:
        sendError(message, 1, 1);
        break;
    }
}

void NullAgent::onFeaturesRequest(const Message *message)
{
    FeaturesReplyBuilder msg{message};
    msg.setFeatures(gFeatures);
    msg.send(message->source());
}


void onFlowMod(const Message *message)
{
    const FlowMod *flowMod = FlowMod::cast(message);
    if (!flowMod) {
        sendError(message, OFPET_BAD_REQUEST, )
        return;
    }
}


void NullAgent::sendError(const Message *message, UInt16 type, UInt16 code)
{
    ErrorBuilder msg{message, type, code};
    msg.send(message->source());
}

#endif // NULLAGENT_H