//  ===== ---- ofp/api/apichannellistener.cpp --------------*- C++ -*- =====  //
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
/// \brief Implements api::ApiChannelListener class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/api/apichannellistener.h"
#include "ofp/api/apiserver.h"

using ofp::api::ApiChannelListener;

void ApiChannelListener::onChannelUp(Channel *channel) {
  assert(channel_ == nullptr);

  channel_ = channel;
  server_->onChannelUp(channel_);
}

void ApiChannelListener::onChannelDown(Channel *channel) {
  assert(channel == channel_);
  server_->onChannelDown(channel_);
}

void ApiChannelListener::onMessage(const Message *message) {
  server_->onMessage(channel_, message);
}

void ApiChannelListener::onTimer(UInt32 timerID) {
  server_->onTimer(channel_, timerID);
}