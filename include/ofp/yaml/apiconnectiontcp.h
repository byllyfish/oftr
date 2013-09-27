//  ===== ---- ofp/yaml/apiconnectiontcp.h -----------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::ApiConnectionTCP class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_APICONNECTIONTCP_H
#define OFP_YAML_APICONNECTIONTCP_H

#include "ofp/yaml/apiconnection.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class ApiConnectionTCP : public ApiConnection {
public:
	ApiConnectionTCP(ApiServer *server, sys::tcp::socket socket);

	void asyncAccept() override;

protected:
	void write(const std::string &msg) override;
	void asyncRead() override;
	void asyncWrite() override;

private:
	sys::tcp::socket socket_;
	boost::asio::streambuf streambuf_;

	// Use a two buffer strategy for async-writes. We queue up data in one
	// buffer while we're in the process of writing the other buffer.
	ByteList outgoing_[2];
	int outgoingIdx_ = 0;
	bool writing_ = false;
};

} // </namespace yaml>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APICONNECTIONTCP_H
