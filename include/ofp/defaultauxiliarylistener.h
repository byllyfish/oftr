//  ===== ---- ofp/defaultauxiliarylistener.h --------------*- C++ -*- =====  //
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
/// \brief Defines the DefaultAuxiliaryListener class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DEFAULTAUXILIARYLISTENER_H_
#define OFP_DEFAULTAUXILIARYLISTENER_H_

#include "ofp/channellistener.h"

namespace ofp {

class DefaultAuxiliaryListener : public ChannelListener {
public:
  virtual void onChannelUp(Channel *channel) override {
    log::debug("DefaultAuxiliaryListener onChannelUp");
  }

  virtual void onMessage(const Message *message) override {
    log::debug("DefaultAuxiliaryListener onMessage");
  }

private:
};

}  // namespace ofp

#endif  // OFP_DEFAULTAUXILIARYLISTENER_H_
