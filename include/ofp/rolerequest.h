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

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class RoleRequest : public ProtocolMsg<RoleRequest,OFPT_ROLE_REQUEST> {
public:
	UInt32 role() const { return role_; }
	UInt64 generationId() const { return generationId_; }
	
	UInt32 xid() const { return header_.xid(); }
	
	bool validateLength(size_t length) const;

private:
	Header header_;
	Big32 role_;
	Padding<4> pad_;
	Big64 generationId_;

	// Only RoleRequestBuilder can construct an instance.
	RoleRequest() : header_{type()} {}

	friend class RoleRequestBuilder;
	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(RoleRequest) == 24, "Unexpected size.");
static_assert(IsStandardLayout<RoleRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<RoleRequest>(), "Expected trivially copyable.");

class RoleRequestBuilder {
public:
	RoleRequestBuilder() = default;
	explicit RoleRequestBuilder(const RoleRequest *msg);

	void setRole(UInt32 role) { msg_.role_ = role; }
	void setGenerationId(UInt64 generationId) { msg_.generationId_ = generationId; }

	UInt32 send(Writable *channel);

private:
	RoleRequest msg_;

	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_ROLEREQUEST_H
