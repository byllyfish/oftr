#ifndef NULLAGENT_H
#define NULLAGENT_H

#include "ofp.h"

using ofp::UInt16;
using ofp::UInt32;
using ofp::Message;
using ofp::Channel;
using std::chrono::milliseconds;


class NullAgent : public ofp::ChannelListener {
public:

    static NullAgent *Factory() { return new NullAgent; }

    void onChannelUp(Channel *channel) override
    {
        ofp::log::debug("NullAgent channel up.");

        channel->openAuxChannel(1, Channel::Transport::TCP);
        //channel->scheduleTimer(0, milliseconds{1290}, true);
    }

    void onChannelDown(Channel *channel) override
    {
        ofp::log::debug("NullAgent channel down.");
    }

    void onMessage(const Message *message) override;

    void onTimer(UInt32 timerID) override 
    {
        ofp::log::debug("NullAgent timer", timerID);
    }

private:
    void onFlowMod(const Message *message);
    void sendError(UInt16 type, UInt16 code, const Message *message);
};


void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    default:
        sendError(1, 1, message);
        break;
    }
}


void NullAgent::onFlowMod(const Message *message)
{
    auto flowMod = ofp::FlowMod::cast(message);
    if (!flowMod) {
        sendError(ofp::OFPET_BAD_REQUEST, ofp::OFPBRC_BAD_TYPE, message);
        return;
    }

    
}

void NullAgent::sendError(UInt16 type, UInt16 code, const Message *message)
{
    ofp::ErrorBuilder msg{type, code, message};
    msg.send(message->source());
}

#endif // NULLAGENT_H
