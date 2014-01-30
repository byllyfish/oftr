//  ===== ---- ofp/yaml/apichannellistener.h ---------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::ApiChannelListener class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_APICHANNELLISTENER_H
#define OFP_YAML_APICHANNELLISTENER_H

#include "ofp/channellistener.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class ApiServer;

class ApiChannelListener : public ChannelListener {
public:
	ApiChannelListener(ApiServer *server): server_{server} {}

	void onChannelUp(Channel *channel) override;
    void onChannelDown(Channel *channel) override;
    void onMessage(const Message *message) override;
    void onTimer(UInt32 timerID) override;

private:
	ApiServer *server_;
	Channel *channel_ = nullptr;
};

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_APICHANNELLISTENER_H
