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
#include "ofp/api/apievents.h"
#include "ofp/api/apiencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"

using ofp::api::ApiConnection;

ApiConnection::ApiConnection(ApiServer *server, bool listening)
    : server_{server}, isListening_{listening} {
  server_->onConnect(this);
}

ApiConnection::~ApiConnection() { server_->onDisconnect(this); }

void ApiConnection::onLoopback(ApiLoopback *loopback) {
  ByteList &buf = loopback->params.data;
  ApiBoolean validate = loopback->params.validate;

  Message message{buf.mutableData(), buf.size()};
  message.transmogrify();

  yaml::Decoder decoder{&message};

  if (validate == LIBOFP_NOT_PRESENT) {
    // Always respond with OpenFlow YAML or a DecodeError event.
    if (decoder.error().empty()) {
      write(decoder.result());
    } else {
      ApiDecodeError reply;
      reply.params.error = decoder.error();
      reply.params.data = RawDataToHex(message.data(), message.size());
      write(reply.toString(isFormatJson_));
    }
  } else if (validate == LIBOFP_FALSE) {
    // We don't expect data to validate; only respond if it does.
    if (decoder.error().empty()) {
      ApiDecodeError reply;
      reply.params.error = "Message unexpectedly validates";
      reply.params.data = RawDataToHex(message.data(), message.size());
      write(reply.toString(isFormatJson_));
    }
  } else {
    // We expect the data to validate; only respond if it doesn't.
    if (!decoder.error().empty()) {
      ApiDecodeError reply;
      reply.params.error = decoder.error();
      reply.params.data = RawDataToHex(message.data(), message.size());
      write(reply.toString(isFormatJson_));
    }
  }
}

void ApiConnection::onListenRequest(ApiListenRequest *listenReq) {
  server_->onListenRequest(this, listenReq);
  isListening_ = true;
}

void ApiConnection::onListenReply(ApiListenReply *listenReply) {
  write(listenReply->toString(isFormatJson_));
}

void ApiConnection::onConnectRequest(ApiConnectRequest *connectReq) {
  server_->onConnectRequest(this, connectReq);
}

void ApiConnection::onConnectReply(ApiConnectReply *connectReply) {
  write(connectReply->toString(isFormatJson_));
}

void ApiConnection::onSetTimer(ApiSetTimer *setTimer) {
  server_->onSetTimer(this, setTimer);
}

void ApiConnection::onEditSetting(ApiEditSetting *editSetting) {
  // Handle change in "format" setting: values are "json" and "yaml". This is
  // the format of data sent by the connection.
  if (editSetting->params.name == "format") {
    if (editSetting->params.value == "json") {
      log::debug("Using JSON.");
      isFormatJson_ = true;
    } else if (editSetting->params.value == "yaml") {
      isFormatJson_ = false;
    }
  }
}

void ApiConnection::onYamlError(const std::string &error,
                                const std::string &text) {
  ApiYamlError reply;
  reply.params.error = error;
  reply.params.text = text;
  write(reply.toString(isFormatJson_));
}

void ApiConnection::onChannelUp(Channel *channel) {
  ApiDatapathUp reply;
  reply.params.datapathId = channel->datapathId();
  reply.params.version = channel->version();
  reply.params.endpoint = channel->remoteEndpoint();

  write(reply.toString(isFormatJson_));
}

void ApiConnection::onChannelDown(Channel *channel) {
  ApiDatapathDown reply;
  reply.params.datapathId = channel->datapathId();
  reply.params.version = channel->version();
  reply.params.endpoint = channel->remoteEndpoint();

  write(reply.toString(isFormatJson_));
}

void ApiConnection::onMessage(Channel *channel, const Message *message) {
  yaml::Decoder decoder{RemoveConst_cast(message), isFormatJson_};

  if (decoder.error().empty()) {
    write(decoder.result());
  } else {
    ApiDecodeError reply;
    reply.params.datapathId = channel->datapathId();
    reply.params.error = decoder.error();
    reply.params.data = RawDataToHex(message->data(), message->size());
    write(reply.toString(isFormatJson_));
  }
}

void ApiConnection::onTimer(Channel *channel, UInt32 timerID) {
  ApiTimer timer;
  timer.params.datapathId = channel->datapathId();
  timer.params.timerId = timerID;
  write(timer.toString(isFormatJson_));
}

void ApiConnection::processInputLine(std::string *line) {
  // Line is modified by this method.
  cleanInputLine(line);

  text_ += *line + '\n';

  if (*line == "---" || *line == "...") {
    handleEvent(text_);
    text_ = "---\n";
  }
}

void ApiConnection::handleEvent(const std::string &eventText) {
  EventType type = eventTypeOf(eventText);
  if (type == EmptyEvent) return;

  if (type == LibEvent) {
    ApiEncoder encoder{eventText, this};

  } else {
    yaml::Encoder encoder(eventText, [this](const DatapathID &datapathId) {
      return server_->findChannel(datapathId);
    });

    if (encoder.error().empty()) {
      if (isListening_) {
        Channel *channel = server_->findChannel(encoder.datapathId());

        if (channel) {
          channel->write(encoder.data(), encoder.size());
          channel->flush();
        } else {
          std::string errorMsg =
              "Unknown Datapath ID: " + encoder.datapathId().toString();
          onYamlError(errorMsg, RawDataToHex(encoder.data(), encoder.size()));
        }

      } else {
        assert(!isListening_);

        // If an OpenFlow YAML message is received before we start
        // listening, return its binary value in a loopback message.
        ApiLoopback reply;
        reply.params.validate = LIBOFP_NOT_PRESENT;
        reply.params.data.set(encoder.data(), encoder.size());
        write(reply.toString(isFormatJson_));
      }

    } else {
      onYamlError(encoder.error(), eventText);
    }
  }
}

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
