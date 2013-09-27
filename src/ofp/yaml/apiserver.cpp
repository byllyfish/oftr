//  ===== ---- ofp/yaml/apiserver.cpp ----------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements ApiServer class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiserver.h"
#include "ofp/yaml/apiconnectiontcp.h"
#include "ofp/yaml/apiconnectionstdio.h"
#include "ofp/sys/engine.h"
#include "ofp/yaml/apichannellistener.h"

using namespace ofp::yaml;
using namespace ofp::sys;

// Unix Stdio descriptors.
const int STDIN = 0;
const int STDOUT = 1;
const int STDERR = 2;

OFP_BEGIN_IGNORE_PADDING

void ApiServer::run(const IPv6Address &localAddress, UInt16 localPort)
{
    Driver driver;
    ApiServer server{&driver, localAddress, localPort};

    driver.run();
}

ApiServer::ApiServer(Driver *driver, const IPv6Address &localAddress,
                     UInt16 localPort)
    : engine_{driver->engine()},
      endpt_{makeTCPEndpoint(localAddress, localPort)},
      acceptor_{engine_->io()}, socket_{engine_->io()}
{
    auto conn = std::make_shared<ApiConnectionStdio>(this, stream_descriptor{engine_->io(), STDIN}, stream_descriptor{engine_->io(), STDOUT});
    conn->asyncAccept();

    if (localPort != 0) {
        acceptor_.bind(endpt_);
        asyncAccept();
        log::info("Start TCP listening on", endpt_);
    }
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
                    std::make_shared<ApiConnectionTCP>(this, std::move(socket_));
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
    // FIXME
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
