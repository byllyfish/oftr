//  ===== ---- ofp/rolerequest.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the RoleRequest and RoleRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ROLEREQUEST_H
#define OFP_ROLEREQUEST_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class RoleRequest {
public:
	enum { Type = OFPT_ROLE_REQUEST };

	static const RoleRequest *cast(const Message *message);

	RoleRequest() : header_{Type} {}

	UInt32 role() const;
	UInt64 generationID() const;
	
private:
	Header header_;
	Big32 role_;
	Padding<4> pad_;
	Big64 generationID_;

	bool validateLength(size_t length) const;

	friend class RoleRequestBuilder;
};

static_assert(sizeof(RoleRequest) == 24, "Unexpected size.");
static_assert(IsStandardLayout<RoleRequest>(), "Expected standard layout.");

class RoleRequestBuilder {
public:
	RoleRequestBuilder() = default;

	void setRole(UInt32 role);
	void setGenerationID(UInt64 generationID);

	UInt32 send(Writable *channel);

private:
	RoleRequest msg_;
};

} // </namespace ofp>

#endif // OFP_ROLEREQUEST_H
