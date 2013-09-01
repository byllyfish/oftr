//  ===== ---- ofp/rolerequest.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
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
