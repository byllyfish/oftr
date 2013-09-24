//  ===== ---- ofp/yaml/apiconnection.cpp ------------------*- C++ -*- =====  //
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
/// \brief Implements yaml::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiconnection.h"
#include "ofp/yaml/apievents.h"
#include "ofp/yaml/apiencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"
#include "ofp/features.h"

using namespace ofp::yaml;
using namespace ofp::sys;

// Return true if first non-whitespace substring in `s` exactly equals `cs`.
static bool startsWith(const std::string &s, const char *cs)
{
    size_t pos = s.find_first_not_of(" \t\f\v");
    if (pos != std::string::npos && s[pos] == *cs) {
        size_t left = s.length() - pos;
        size_t len = strlen(cs);
        if (len <= left) {
            return std::memcmp(cs, &s[pos], len) == 0;
        }
    }
    return false;
}

ApiConnection::ApiConnection(ApiServer *server, sys::tcp::socket socket)
    : server_{server}, socket_{std::move(socket)}
{
    server_->onConnect(this);
}

ApiConnection::~ApiConnection()
{
    server_->onDisconnect(this);
}

void ApiConnection::onListenRequest(ApiListenRequest *listenReq)
{
    server_->onListenRequest(this, listenReq);
}

void ApiConnection::onListenReply(ApiListenReply *listenReply)
{
    write(listenReply->toString());
}

void ApiConnection::onSetTimer(ApiSetTimer *setTimer)
{
    server_->onSetTimer(this, setTimer);
}

void ApiConnection::onYamlError(const std::string &error, const std::string &text)
{
    // YAMLIO doesn't really output strings with newlines AFAICT that well.
    // I am escaping the newlines with \n for error messages and original
    // event text.

    ApiYamlError reply;
    reply.msg.error = escape(error);
    reply.msg.text = escape(text);
    write(reply.toString());
}

void ApiConnection::onChannelUp(Channel *channel)
{
    ApiDatapathUp reply;
    reply.msg.datapathId = channel->datapathId();
    reply.msg.version = channel->version();

    const Features &features = channel->features();
    reply.msg.bufferCount = features.bufferCount();
    reply.msg.tableCount = features.tableCount();
    reply.msg.capabilities = features.capabilities();
    reply.msg.reserved = features.reserved();

    write(reply.toString());
}

void ApiConnection::onChannelDown(Channel *channel)
{
    ApiDatapathDown reply;
    reply.msg.datapathId = channel->datapathId();
    write(reply.toString());
}

void ApiConnection::onMessage(Channel *channel, const Message *message)
{
    Decoder decoder{const_cast<Message *>(message)};

    if (decoder.error().empty()) {
        write(decoder.result());
    } else {
        ApiDecodeError reply;
        reply.msg.datapathId = channel->datapathId();
        reply.msg.error = decoder.error();
        reply.msg.data = RawDataToHex(message->data(), message->size());
        write(reply.toString());
    }
}

void ApiConnection::onException(Channel *channel, const Exception *exception)
{
    
}

void ApiConnection::onTimer(Channel *channel, UInt32 timerID)
{
    ApiTimer timer;
    timer.msg.datapathId = channel->datapathId();
    timer.msg.timerId = timerID;
    write(timer.toString());
}

void ApiConnection::write(const std::string &msg)
{
    outgoing_[outgoingIdx_].add(msg.data(), msg.length());
    if (!writing_) {
        asyncWrite();
    }
}

void ApiConnection::asyncAccept()
{
    // Do nothing if socket is not open.
    if (!socket_.is_open())
        return;

    // We always send and receive complete messages; disable Nagle algorithm.
    socket_.set_option(tcp::no_delay(true));

    // Start first async read.
    asyncRead();
}

void ApiConnection::asyncRead()
{
    auto self(shared_from_this());

    boost::asio::async_read_until(
        socket_, streambuf_, '\n',
        [this, self](const error_code & err, size_t bytes_transferred) {
            if (!err) {
                std::istream is(&streambuf_);
                std::string line;
                std::getline(is, line);
                cleanInputLine(&line);
                handleInputLine(line);
                asyncRead();
            } else if (!isAsioEOF(err)) {
                auto exc = makeException(err);
                log::info("ApiConnection::asyncRead err", exc);
            }
        });
}

void ApiConnection::asyncWrite()
{
    assert(!writing_);

    int idx = outgoingIdx_;
    outgoingIdx_ = !outgoingIdx_;
    writing_ = true;

    const UInt8 *data = outgoing_[idx].data();
    size_t size = outgoing_[idx].size();

    auto self(shared_from_this());

    boost::asio::async_write(socket_, boost::asio::buffer(data, size), [this, self](const error_code &err, size_t bytes_transferred) {

        if (!err) {
            assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

            writing_ = false;
            outgoing_[!outgoingIdx_].clear();
            if (outgoing_[outgoingIdx_].size() > 0) {
                // Start another async write for the other output buffer.
                asyncWrite();
            }

        } else {
            log::debug("Write error ", makeException(err));
        }
    });
}

void ApiConnection::handleInputLine(const std::string &line)
{
    text_ += line + '\n';

    if (line == "---" || line == "...") {
        handleEvent();
        text_ = "---\n";
        isLibEvent_ = false;
        lineCount_ = 0;
    } else if (lineCount_ == 1 && startsWith(line, "event:")) {
        // If first line starts with 'event:', we have
        // a library event, not an OpenFlow message.
        isLibEvent_ = true;
    }

    ++lineCount_;
}


void ApiConnection::handleEvent()
{
    if (isLibEvent_) {
        ApiEncoder encoder{text_, this};

    } else {
        // Ignore event if there is no data.
        if (isEmptyEvent(text_))
            return;

        Encoder encoder(text_, [this](const DatapathID &datapathId) {
            return server_->findChannel(datapathId);
        });

        if (encoder.error().empty()) {
            Channel *channel = server_->findChannel(encoder.datapathId());

            if (channel) {
                channel->write(encoder.data(), encoder.size());
                channel->flush();
            } else {
                std::string errorMsg = "Unknown Datapath ID: " + encoder.datapathId().toString();
                onYamlError(errorMsg, RawDataToHex(encoder.data(), encoder.size()));
            }

        } else {
            onYamlError(encoder.error(), text_);
        }
    }
}


void ApiConnection::cleanInputLine(std::string *line)
{
    // For telnet clients, handle CR-LF just in case.
    if (!line->empty() && line->back() == '\r') {
        line->pop_back();
    }

    // Replace non-ASCII and non-white-space control characters with '~'.
    // Replace carriage returns with linefeeds.
    for (char &ch : *line) {
        if ((ch & 0x80) || (ch < 32 && !std::isspace(ch))) {
            ch = '~';
        } else if (ch == '\r') {
            ch = '\n';
        }
    }
}


bool ApiConnection::isEmptyEvent(const std::string &s)
{
    // String should have the format:  "---\n(...)\n---\n"
    // Return true if string is too short or there is no colon.
    
    if (s.size() < 12) 
        return true;

    if (s.find(':') == std::string::npos)
        return true;

    return false;
}

// Replace newlines in `s` with explicit '\n' escape sequence.
std::string ApiConnection::escape(const std::string &s)
{
    std::string result;
    result.reserve(s.length());

    for (auto ch : s) {
        if (ch == '\n') {
            result += "\\n";
        } else {
            result += ch;
        }
    }
    return result;
}
