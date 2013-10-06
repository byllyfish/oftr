#include "bridgelistener.h"

using namespace bridge;


BridgeListener::BridgeListener(BridgeListener *otherBridge)
    : otherBridge_{otherBridge}
{
    assert(!remoteAddr_.valid());
}

BridgeListener::BridgeListener(const IPv6Address &remoteAddr)
    : remoteAddr_{remoteAddr}
{
    assert(otherBridge_ == nullptr);
}


BridgeListener::~BridgeListener()
{
    if (otherBridge_) {
        otherBridge_->forget(this);
    }
}

void BridgeListener::onChannelUp(Channel *channel)
{
    assert(channel_ == nullptr);

    channel_ = channel;

    if (remoteAddr_.valid()) {
        assert(otherBridge_ == nullptr);

        // If we're the incoming side, connect to remote address and set
        // up bridge.

        Driver *driver = channel->driver();

        // Create a bridge listener pointing back to us. Set up
        // connection so it will use this `other` bridge listener. Each bridge
        // listener will release the other's pointer to it in its destructor.

        BridgeListener *bridge = new BridgeListener(this);
        otherBridge_ = bridge;

        auto exc = driver->connect(Driver::Bridge, nullptr, remoteAddr_,
                                   Driver::DefaultPort, ProtocolVersions::All,
                                   [bridge]() { return bridge; });

        exc.done([bridge](Exception ex) {
            // If connection fails, delete the otherBridge_.
            if (ex) {
                delete bridge;
            }
        });

    } else {
        // We're the outgoing side.

        // If otherBridge_ is null, it means that the incoming side has
        // gone down. Close the channel.
        if (otherBridge_ == nullptr) {
            log::debug("null bridge in onChannelUp - close channel.");
            channel_->shutdown();
        }
    }
}

void BridgeListener::onMessage(const Message *message)
{
    translateAndForward(message, otherBridge_->channel());
}

void BridgeListener::forget(BridgeListener *other)
{
    assert(otherBridge_ == other);

    otherBridge_ = nullptr;
}

template <class MsgType, class MsgBuilder>
static void translateFwd(const Message *message, Channel *channel)
{
    const MsgType *m = message->cast<MsgType>();
    if (m) {
        MsgBuilder builder{m};
        builder.send(channel);
    } else {
        log::info("BridgeListener: Unable to decode message", MsgType::type());
    }
}

#define CASE_MSG(MsgType)                                                      \
    case MsgType::type() :                                                     \
        translateFwd<MsgType, MsgType##Builder>(message, channel);             \
        break

void BridgeListener::translateAndForward(const Message *message,
                                         Channel *channel)
{

    switch (message->type()) {
        CASE_MSG(Hello);
        CASE_MSG(Error);
        CASE_MSG(Experimenter);
        CASE_MSG(FeaturesRequest);
        CASE_MSG(FeaturesReply);
        CASE_MSG(GetConfigRequest);
        CASE_MSG(GetConfigReply);
        CASE_MSG(SetConfig);
        CASE_MSG(PacketIn);
        CASE_MSG(FlowRemoved);
        CASE_MSG(PortStatus);
        CASE_MSG(PacketOut);
        CASE_MSG(FlowMod);
        CASE_MSG(GroupMod);
        CASE_MSG(PortMod);
        CASE_MSG(TableMod);
#if 0
        CASE_MSG(MultipartRequest);
        CASE_MSG(MultipartReply);
        CASE_MSG(BarrierRequest);
        CASE_MSG(BarrierReply);
        //CASE_MSG(QueueGetConfigRequest);
        //CASE_MSG(QueueGetConfigReply);
        CASE_MSG(RoleRequest);
        CASE_MSG(RoleReply);
        CASE_MSG(GetAsyncRequest);
        CASE_MSG(GetAsyncReply);
        CASE_MSG(SetAsync);
        //CASE_MSG(MeterMod);
#endif //0
    default:
        log::info("Unrecognized message type:", message->type());
        break;
    }
}

