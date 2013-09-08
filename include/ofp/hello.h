//  ===== ---- ofp/hello.h ---------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HELLO_H
#define OFP_HELLO_H

#include "ofp/header.h"
#include "ofp/protocolversions.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

namespace detail { // <namespace detail>

class HelloElement {
public:
	UInt16 type() const;

	void setType(UInt16 type);
	void setLength(UInt16 length);
	
	ProtocolVersions versionBitMap() const;
	bool validateLength(size_t remaining) const;
	const HelloElement *next(size_t *remaining) const;

private:
	Big16	type_;
	Big16	length_;
};

} // </namespace detail>


/// \brief Immutable OpenFlow `Hello` protocol message.
class Hello {
public:
	static constexpr OFPType type() { return OFPT_HELLO; }

	Hello() : header_{type()} {}

	static const Hello *cast(const Message *message);

	ProtocolVersions protocolVersions() const;

private:
	Header header_;

	bool validateLength(size_t length) const;
	const detail::HelloElement *helloElements() const;

	friend class HelloBuilder;
};

static_assert(sizeof(Hello) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Hello>(), "Expected standard layout.");

/// \brief Mutable builder for an OpenFlow `Hello` protocol message.
class HelloBuilder {
public:

	HelloBuilder(const Header &header) {
		msg_.header_ = header;
	}

	HelloBuilder(ProtocolVersions versions) : bitmap_{versions.bitmap()} {
		msg_.header_.setVersion(versions.highestVersion());
	}

	void setProtocolVersions(ProtocolVersions versions) {
		bitmap_ = versions.bitmap();
	}

	UInt32 send(Writable *channel);

private:
	Hello msg_;
	detail::HelloElement elem_;
	Big32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_HELLO_H
