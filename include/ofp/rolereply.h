//  ===== ---- ofp/rolereply.h -----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
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
