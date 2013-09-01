//  ===== ---- ofp/port.h ----------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Port class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PORT_H
#define OFP_PORT_H

#include "ofp/byteorder.h"
#include "ofp/smallcstring.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class Port {
public:
	Port();

private:
	Big32 portNo_;
	Padding<4> pad1_;
	EnetAddress hwAddr_;
	Padding<2> pad2_;
	SmallCString<OFP_MAX_PORT_NAME_LEN> name_;
	Big32 config_;
	Big32 state_;
	Big32 curr_;
	Big32 advertised_;
	Big32 supported_;
	Big32 peer_;
	Big32 currSpeed_;
	Big32 maxSpeed_;
};

static_assert(sizeof(Port) == 64, "Unexpected size.");
static_assert(IsStandardLayout<Port>(), "Expected standard layout.");

} // </namespace ofp>

#endif // OFP_PORT_H
