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
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class Message;

class EchoReply {
public:
	static constexpr OFPType type() { return OFPT_ECHO_REPLY; }
	static const EchoReply *cast(const Message *message);
	
	EchoReply() : header_{type()} {}

	ByteRange echoData() const;

	bool validateLength(size_t length) const;

private:
	Header header_;

	friend class EchoReplyBuilder;
};


class EchoReplyBuilder {
public:
	explicit EchoReplyBuilder(UInt32 xid);

	void setEchoData(const void *data, size_t length) {
		data_.set(data, length);
	}
	
	void send(Writable *channel);

private:
	EchoReply msg_;
	ByteList data_;
};

} // </namespace ofp>

#endif // OFP_ECHOREPLY_H
