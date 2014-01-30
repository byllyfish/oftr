//  ===== ---- ofp/channellistener.cpp ---------------------*- C++ -*- =====  //
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
/// \brief Implements ChannelListener base class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/channellistener.h"
#include "ofp/log.h"

using namespace ofp;

void ChannelListener::onChannelUp(Channel *channel) {
  log::debug("ChannelListener - onChannelUp ignored:", channel);
}

void ChannelListener::onChannelDown(Channel *channel) {
  log::debug("ChannelListener - onChannelDown ignored:", channel);
}

void ChannelListener::onTimer(UInt32 timerID) {
  log::debug("ChannelListener - onTimer ignored:", timerID);
}

void ChannelListener::dispose(ChannelListener *listener) { delete listener; }
