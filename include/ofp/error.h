//  ===== ---- ofp/error.h ---------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Error and ErrorBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ERROR_H
#define OFP_ERROR_H

#include "ofp/header.h"
#include "ofp/channel.h"

namespace ofp { // <namespace ofp>

class Message;

class Error {
public:
	enum { Type = OFPT_ERROR };

	Error() : header_{Type} {}

private:
	Header header_;
	Big16 type_;
	Big16 code_;

	friend class ErrorBuilder;
};


static_assert(sizeof(Error) == 12, "Unexpected size.");


class ErrorBuilder {
public:
	ErrorBuilder(UInt16 type, UInt16 code, const Message *message);

	void send(Channel *channel);

private:
	Error msg_;
};

} // </namespace ofp>

#endif // OFP_ERROR_H
