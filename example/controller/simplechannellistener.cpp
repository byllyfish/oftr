#include "simplechannellistener.h"

namespace controller { // <namespace controller>


void SimpleChannelListener::onChannelUp(Channel *channel)
{
    UInt32 xid;

    xid = GetAsyncRequestBuilder{}.send(channel);
    trackReply(xid, &SimpleChannelListener::onGetAsyncReply);

    xid = GetConfigRequestBuilder{}.send(channel);
    trackReply(xid, &SimpleChannelListener::onGetConfigReply);
}

void SimpleChannelListener::onMessage(const Message *message)
{
    switch (message->type()) {
    case PacketIn::Type:
        if (auto msg = PacketIn::cast(message))
            controller_.onPacketIn(message->source(), msg);
        break;

    default:
        onReply(message);
        break;
    }
}


void SimpleChannelListener::onGetAsyncReply(const GetAsyncReply *msg)
{

}


void SimpleChannelListener::onGetConfigReply(const GetConfigReply *msg)
{

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

