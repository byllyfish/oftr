//  ===== ---- ofp/echoreply.h -----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the EchoReply and EchoReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ECHOREPLY_H
#define OFP_ECHOREPLY_H

#include "ofp/header.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class EchoReply {
public:
	static constexpr OFPType type() { return OFPT_ECHO_REPLY; }
	const EchoReply *cast(const Message *message);
	
	EchoReply() : header_{type()} {}

private:
	Header header_;
};


class EchoReplyBuilder {
public:
	explicit EchoReplyBuilder(const EchoRequest *request);

	void send(Writable *channel);

private:
	EchoReply msg_;
};

} // </namespace ofp>

#endif // OFP_ECHOREPLY_H
