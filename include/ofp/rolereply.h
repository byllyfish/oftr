//  ===== ---- ofp/rolereply.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the RoleReply and RoleReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ROLEREPLY_H
#define OFP_ROLEREPLY_H

namespace ofp { // <namespace ofp>

class RoleReply {
public:
	enum { Type = OFPT_ROLE_REPLY };

	static const RoleReply *cast(const Message *message);

	RoleReply() : header_{Type} {}

	UInt32 role() const;
	UInt64 generationID() const;

private:
	Header header_;
	Big32 role_;
	Padding<4> pad_;
	Big64 generationID_;

	bool validateLength(size_t length) const;

	friend class RoleReplyBuilder;
};

static_assert(sizeof(RoleReply) == 24, "Unexpected size.");
static_assert(IsStandardLayout<RoleReply>(), "Expected standard layout.");

class RoleReplyBuilder {
public:
	explicit RoleReplyBuilder(const RoleRequest *request);

	void setRole(UInt32 role);
	void setGenerationID(UInt64 generationID);

	UInt32 send(Writable *channel);

private:
	RoleReply msg_;
};

} // </namespace ofp>

#endif // OFP_ROLEREPLY_H
