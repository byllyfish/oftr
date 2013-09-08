//  ===== ---- ofp/echorequest.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the EchoRequest and EchoRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ECHOREQUEST_H
#define OFP_ECHOREQUEST_H

#include "ofp/header.h"

namespace ofp { // <namespace ofp>

class Message;
class Writable;

class EchoRequest {
public:
	static constexpr OFPType type() { return OFPT_ECHO_REQUEST; }
	static const EchoRequest *cast(const Message *message);

	EchoRequest() : header_{type()} {}

private:
	Header header_;

	bool validateLength(size_t length) const;

	friend class EchoRequestBuilder;
};


class EchoRequestBuilder {
public:
	EchoRequestBuilder() = default;

	void send(Writable *channel);
	
private:
	EchoRequest msg_;
};

} // </namespace ofp>

#endif // OFP_ECHOREQUEST_H
