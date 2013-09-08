#include "simplechannellistener.h"

namespace controller { // <namespace controller>

void SimpleChannelListener::onChannelUp(Channel *channel)
{
    UInt32 xid;

    log::debug("SimpleChannelListener UP");

    SetConfigBuilder config;
    config.setMissSendLen(14); // ethernet header only
    config.send(channel);
    // No Reply expected.

    FlowModBuilder flowMod;
    flowMod.setTableId(OFPTT_ALL);
    flowMod.setCommand(OFPFC_DELETE);
    flowMod.setPriority(0x8000);
    flowMod.setBufferId(OFP_NO_BUFFER);
    flowMod.setOutPort(OFPP_ANY);
    flowMod.setOutGroup(OFPG_ANY);
    (void)flowMod.send(channel);

    // No reply expected.

    // xid = GetAsyncRequestBuilder{}.send(channel);
    // trackReply(xid, &SimpleChannelListener::onGetAsyncReply);

    // xid = GetConfigRequestBuilder{}.send(channel);
    // trackReply(xid, &SimpleChannelListener::onGetConfigReply);

    BarrierRequestBuilder barrier;
    xid = barrier.send(channel);
    trackReply(xid, &SimpleChannelListener::onBarrierReply);
}

void SimpleChannelListener::onMessage(const Message *message)
{
    switch (message->type()) {
    case PacketIn::Type:
        if (auto msg = PacketIn::cast(message))
            controller_.onPacketIn(message->source(), msg);
        break;

    case PortStatus::Type:
        if (auto msg = PortStatus::cast(message))
            controller_.onPortStatus(message->source(), msg);
        break;

    case Error::Type:
        if (auto msg = Error::cast(message))
            controller_.onError(message->source(), msg);
        break;

    default:
        onReply(message);
        break;
    }
}

void SimpleChannelListener::onGetAsyncReply(const GetAsyncReply *msg)
{
    log::debug("GetAsyncReply");
}

void SimpleChannelListener::onGetConfigReply(const GetConfigReply *msg)
{
    log::debug("GetConfigReply");
}

void SimpleChannelListener::onBarrierReply(const BarrierReply *msg)
{
    log::debug("BarrierReply");
}

void SimpleChannelListener::onReply(const Message *message)
{
    auto xid = message->xid();
    auto iter = tracker_.find(xid);
    if (iter != tracker_.end()) {
        auto callback = iter->second;
        callback(message);
        tracker_.erase(iter);
    } else {
        log::debug("Untracked XID:", xid);
    }
}

} // </namespace controller>
