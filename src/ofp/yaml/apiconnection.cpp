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

// Return true if first alphabetical substring matches "event" or "params".
// Comparison is case-sensitive. We need to skip over initial spaces, { and ".
static bool matchesEventPrefix(const std::string &s) {
  size_t pos = s.find_first_not_of(" \t\f\v{\"");
  if (pos != std::string::npos && s.length() - pos > 6) {
    return (std::memcmp("event", &s[pos], 5) == 0) ||
            (std::memcmp("params", &s[pos], 6) == 0);
  }
  return false;
}

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

  Decoder decoder{&message};

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
  log::debug("ApiConnection::onListenReply");
  write(listenReply->toString(isFormatJson_));
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
  // YAMLIO doesn't really output strings with newlines AFAICT that well.
  // I am escaping the newlines with \n for error messages and original
  // event text.

  ApiYamlError reply;
  reply.params.error = escape(error);
  reply.params.text = escape(text);
  write(reply.toString(isFormatJson_));
}

void ApiConnection::onChannelUp(Channel *channel) {
  ApiDatapathUp reply;
  reply.params.datapathId = channel->datapathId();
  reply.params.version = channel->version();

  const Features &features = channel->features();
  reply.params.bufferCount = features.bufferCount();
  reply.params.tableCount = features.tableCount();
  reply.params.capabilities = features.capabilities();
  reply.params.reserved = features.reserved();

  write(reply.toString(isFormatJson_));
}

void ApiConnection::onChannelDown(Channel *channel) {
  ApiDatapathDown reply;
  reply.params.datapathId = channel->datapathId();
  write(reply.toString(isFormatJson_));
}

void ApiConnection::onMessage(Channel *channel, const Message *message) {
  Decoder decoder{RemoveConst_cast(message), isFormatJson_};

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

void ApiConnection::onException(Channel *channel, const Exception *exception) {}

void ApiConnection::onTimer(Channel *channel, UInt32 timerID) {
  ApiTimer timer;
  timer.params.datapathId = channel->datapathId();
  timer.params.timerId = timerID;
  write(timer.toString(isFormatJson_));
}

void ApiConnection::handleInputLine(std::string *line) {
  // Line is modified by this method.
  cleanInputLine(line);

  text_ += *line + '\n';

  if (*line == "---" || *line == "...") {
    handleEvent();
    text_ = "---\n";
    isLibEvent_ = false;
    lineCount_ = 0;
  } else if (lineCount_ == 1 && matchesEventPrefix(*line)) {
    // If first line starts with 'event:', we have
    // a library event, not an OpenFlow message.
    isLibEvent_ = true;
  }

  ++lineCount_;
}

void ApiConnection::handleEvent() {
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
      onYamlError(encoder.error(), text_);
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

  if (s.size() < 12)
    return true;

  if (s.find(':') == std::string::npos)
    return true;

  return false;
}

// Replace newlines in `s` with explicit '\n' escape sequence.
std::string ApiConnection::escape(const std::string &s) {
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
