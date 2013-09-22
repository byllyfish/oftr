//  ===== ---- ofp/apiserver.cpp ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements ApiServer class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiserver.h"
#include "ofp/yaml/apiconnection.h"
#include "ofp/sys/engine.h"
#include "ofp/yaml/apichannellistener.h"

using namespace ofp::yaml;
using namespace ofp::sys;

OFP_BEGIN_IGNORE_PADDING

void ApiServer::run(const IPv6Address &localAddress, UInt16 localPort)
{
    Driver driver;
    ApiServer server{&driver, IPv6Address{}, 9191};

    driver.run();
}

ApiServer::ApiServer(Driver *driver, const IPv6Address &localAddress,
                     UInt16 localPort)
    : engine_{driver->engine()},
      endpt_{makeTCPEndpoint(localAddress, localPort)},
      acceptor_{engine_->io(), endpt_}, socket_{engine_->io()}
{
    asyncAccept();
    log::info("Start TCP listening on", endpt_);
}

void ApiServer::asyncAccept()
{
    acceptor_.async_accept(socket_, [this](error_code err) {
        // N.B. ASIO still sends a cancellation error even after
        // async_accept() throws an exception. Check for cancelled operation
        // first; our ApiServer instance will have been destroyed.
        if (isAsioCanceled(err)) {
            return;
        }
        if (!err) {
            // Only allow one connection at a time.
            if (oneConn_ == nullptr) {
                auto conn =
                    std::make_shared<ApiConnection>(this, std::move(socket_));
                conn->asyncAccept();
            } else {
                socket_.close();
            }

        } else {
            Exception exc = makeException(err);
            log::error("Error in ApiServer.asyncAcept:", exc.toString());
        }
        asyncAccept();
    });
}

void ApiServer::onConnect(ApiConnection *conn)
{
    assert(oneConn_ == nullptr);

    oneConn_ = conn;
}

void ApiServer::onDisconnect(ApiConnection *conn)
{
    assert(oneConn_ == conn);

    oneConn_ = nullptr;
}

void ApiServer::onListenRequest(ApiConnection *conn, ApiListenRequest *listenReq)
{   
    Driver *driver = engine_->driver();
    UInt16 listenPort = listenReq->msg.listenPort;

    auto exc = driver->listen(Driver::Controller, nullptr, IPv6Address{}, listenPort, ProtocolVersions{}, [this]() {
        return new ApiChannelListener{this};
    });


    exc.done([this,listenPort](Exception ex) {
        ApiListenReply reply;
        reply.msg.listenPort = listenPort;
        if (ex) {
            reply.msg.error = ex.toString();
        }
        onListenReply(&reply);
    });
}


void ApiServer::onListenReply(ApiListenReply *listenReply)
{
    if (oneConn_) {
        oneConn_->onListenReply(listenReply);
    }
}

void ApiServer::onSetTimer(ApiConnection *conn, ApiSetTimer *setTimer)
{

}

void ApiServer::onChannelUp(Channel *channel)
{
    registerChannel(channel);

    if (oneConn_)
        oneConn_->onChannelUp(channel);
}

void ApiServer::onChannelDown(Channel *channel)
{
    if (oneConn_) 
        oneConn_->onChannelDown(channel);

    unregisterChannel(channel);
}

void ApiServer::onMessage(Channel *channel, const Message *message)
{
    if (oneConn_)
        oneConn_->onMessage(channel, message);
}

void ApiServer::onException(Channel *channel, const Exception *exception)
{
    if (oneConn_)
        oneConn_->onException(channel, exception);
}

void ApiServer::onTimer(Channel *channel, UInt32 timerID)
{
    if (oneConn_)
        oneConn_->onTimer(channel, timerID);
}

void ApiServer::registerChannel(Channel *channel)
{
    assert(datapathMap_.find(channel->datapathId()) == datapathMap_.end());

    datapathMap_[channel->datapathId()] = channel;
}

void ApiServer::unregisterChannel(Channel *channel)
{
    datapathMap_.erase(channel->datapathId());
}


ofp::Channel *ApiServer::findChannel(const DatapathID &datapathId)
{
    auto iter = datapathMap_.find(datapathId);
    if (iter != datapathMap_.end()) {
        return iter->second;
    }

    return nullptr;
}


OFP_END_IGNORE_PADDING
