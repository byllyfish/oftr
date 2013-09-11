//  ===== ---- ofp/header.h --------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Header class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HEADER_H
#define OFP_HEADER_H

#include "ofp/byteorder.h"
#include "ofp/constants.h"


namespace ofp { // <namespace ofp>

class Header {
public:
	Header(OFPType type) : type_{type} {}

	UInt8 version() const { return version_; }
	void setVersion(UInt8 version) { version_ = version; }

	OFPType type() const { return type_; }
	void setType(OFPType type) { type_ = type; }

	UInt16 length() const { return length_; }
	void setLength(UInt16 length) { length_ = length; }

	UInt32 xid() const { return xid_; }
	void setXid(UInt32 xid) { xid_ = xid; }

	static OFPType translateType(UInt8 version, UInt8 type, UInt8 newVersion);
	
private:
	Big8 version_ = 0;			// OFP_VERSION.
	OFPType type_;				// One of the OFPT_ constants.
	Big16 length_ = 0;			// Length including this ofp_header.
	Big32 xid_ = 0;				// Transaction id for this packet.

	template<class T>
	friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(Header) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Header>(), "Expected standard layout.");

} // </namespace ofp>

#endif // OFP_HEADER_H
