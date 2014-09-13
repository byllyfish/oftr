//  ===== ---- ofp/api/apiconnection.cpp -------------------*- C++ -*- =====  //
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
/// \brief Implements api::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/api/apiconnection.h"
#include "ofp/api/rpcencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"

using ofp::api::ApiConnection;

ApiConnection::ApiConnection(ApiServer *server)
    : server_{server} {
  server_->onConnect(this);
}

ApiConnection::~ApiConnection() { 
  server_->onDisconnect(this); 
}

void ApiConnection::onRpcListen(RpcListen *listen) {
  server_->onRpcListen(this, listen);
}

void ApiConnection::onRpcClose(RpcClose *close) {
  server_->onRpcClose(this, close);
}

void ApiConnection::onRpcSend(RpcSend *send) {
  server_->onRpcSend(this, send);
}

void ApiConnection::onRpcSetTimer(RpcSetTimer *setTimer) {
  server_->onRpcSetTimer(this, setTimer);
}

void ApiConnection::onRpcConfig(RpcConfig *config) {

}

void ApiConnection::onRpcListConns(RpcListConns *list) {
  server_->onRpcListConns(this, list);
}

void ApiConnection::onChannelUp(Channel *channel) {
  RpcDatapath notification;
  notification.params.datapathId = channel->datapathId();
  notification.params.version = channel->version();
  notification.params.endpoint = channel->remoteEndpoint();
  notification.params.status = "UP";
  rpcReply(&notification);
}

void ApiConnection::onChannelDown(Channel *channel) {
  RpcDatapath notification;
  notification.params.datapathId = channel->datapathId();
  notification.params.version = channel->version();
  notification.params.endpoint = channel->remoteEndpoint();
  notification.params.status = "DOWN";
  rpcReply(&notification);
}

void ApiConnection::onMessage(Channel *channel, const Message *message) {
  yaml::Decoder decoder{RemoveConst_cast(message), true};

  if (decoder.error().empty()) {
    // Send `ofp.message` notification event.
    write("{\"params\":");
    write(decoder.result());
    write(",\"method\":\"ofp.message\"}\n");

  } else {
    // Send `ofp.message_error` notification event.
    RpcMessageError messageError;
    messageError.params.datapathId = channel->datapathId();
    messageError.params.error = decoder.error();
    messageError.params.data = { message->data(), message->size() };
    rpcReply(&messageError);
  }
}

void ApiConnection::onTimer(Channel *channel, UInt32 timerID) {
  // Send `ofp.timer` notification event.
  RpcTimer timer;
  timer.params.datapathId = channel->datapathId();
  timer.params.timerId = timerID;
  rpcReply(&timer);
}

#if 0
void ApiConnection::processInputLine(std::string *line) {
  // Line is modified by this method.
  cleanInputLine(line);

  text_ += *line + '\n';

  if (*line == "---" || *line == "...") {
    handleEvent(text_);
    text_ = "---\n";
  }
}
#endif //0

void ApiConnection::handleEvent(const std::string &eventText) {

  RpcEncoder encoder{eventText, this, [this](const DatapathID &datapathId) {
      return server_->findChannel(datapathId);
    }};


#if 0
  EventType type = eventTypeOf(eventText);
  if (type == EmptyEvent) return;

  if (type == LibEvent) {
    ApiEncoder encoder{eventText, this};

  } else {
    yaml::Encoder encoder(eventText, true, 0, [this](const DatapathID &datapathId) {
      return server_->findChannel(datapathId);
    });

    if (encoder.error().empty()) {

      if (isLoopbackMode_) {
        // If an OpenFlow YAML message is received in loopback mode,
        // return its binary value in a loopback message.
        ApiLoopback reply;
        reply.params.validate = LIBOFP_NOT_PRESENT;
        reply.params.data.set(encoder.data(), encoder.size());
        write(reply.toString(isFormatJson_));
      } else {
        // Otherwise, find the channel for the datapath and write the message
        // out that channel.
        Channel *channel = server_->findChannel(encoder.datapathId());
        if (channel) {
          channel->write(encoder.data(), encoder.size());
          channel->flush();
        } else {
          std::string errorMsg =
              "Unknown Datapath ID: " + encoder.datapathId().toString();
          onYamlError(errorMsg, RawDataToHex(encoder.data(), encoder.size()));
        }
      }

    } else {
      onYamlError(encoder.error(), eventText);
    }
  }
#endif //0
}

#if 0
void ApiConnection::cleanInputLine(std::string *line) {
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

bool ApiConnection::isEmptyEvent(const std::string &s) {
  // String should have the format:  "---\n(...)\n---\n"
  // Return true if string is too short or there is no colon.

  if (s.size() < 12) return true;

  if (s.find(':') == std::string::npos) return true;

  return false;
}

// Return true if first alphabetical substring matches "event" or "params".
// Comparison is case-sensitive. We need to skip over initial spaces, { and ".
// (We also skip over --- and ...)
static bool matchesEventPrefix(const std::string &s) {
  size_t pos = s.find_first_not_of(" \n\t\f\v{\"-.");
  if (pos != std::string::npos && s.length() - pos > 6) {
    return (std::memcmp("event", &s[pos], 5) == 0) ||
           (std::memcmp("params", &s[pos], 6) == 0);
  }
  return false;
}

/// \returns type of event for event `s`.
ApiConnection::EventType ApiConnection::eventTypeOf(const std::string &s) {
  // String should have the format:  "---\n(...)\n---\n"
  // Return true if string is too short or there is no colon.

  if (s.size() < 12 || s.find(':') == std::string::npos) return EmptyEvent;

  if (matchesEventPrefix(s)) return LibEvent;

  return MsgEvent;
}

#endif //0
#
