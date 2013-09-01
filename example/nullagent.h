#ifndef NULLAGENT_H
#define NULLAGENT_H

#include "ofp.h"
#include "ofp/yaml/ofp_yaml.h"
#include "ofp/yaml/flowmod.h"

using ofp::UInt16;
using ofp::UInt32;
using ofp::Message;
using ofp::Channel;
using ofp::milliseconds;


class NullAgent : public ofp::ChannelListener {
public:

    static NullAgent *Factory() { return new NullAgent; }

    void onChannelUp(Channel *channel) override
    {
        ofp::log::debug("NullAgent channel up.");

        // FIXME support binding of related connections.
        channel->openAuxChannel(1, Channel::Transport::TCP);
        
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
    void onSetConfig(const Message *message);
    void onFlowMod(const Message *message);
    void sendError(UInt16 type, UInt16 code, const Message *message);
};


void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    case ofp::SetConfig::Type:
        onSetConfig(message);
        break;

    case ofp::FlowMod::Type:
        onFlowMod(message);
        break;

    default:
        sendError(1, 1, message);
        break;
    }
}


void NullAgent::onSetConfig(const Message *message)
{
    auto setConfig = ofp::SetConfig::cast(message);
    if (!setConfig) {
        sendError(ofp::OFPET_BAD_REQUEST, ofp::OFPBRC_BAD_TYPE, message);
        return;
    }

    ofp::log::debug(ofp::yaml::write(setConfig));
}


void NullAgent::onFlowMod(const Message *message)
{
    auto flowMod = ofp::FlowMod::cast(message);
    if (!flowMod) {
        sendError(ofp::OFPET_BAD_REQUEST, ofp::OFPBRC_BAD_TYPE, message);
        return;
    }

    ofp::log::debug(ofp::yaml::write(flowMod));
}

void NullAgent::sendError(UInt16 type, UInt16 code, const Message *message)
{
    ofp::ErrorBuilder msg{type, code, message};
    msg.send(message->source());
}

#endif // NULLAGENT_H
