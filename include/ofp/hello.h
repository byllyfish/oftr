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


class Hello {
public:

	enum {
		Type = OFPT_HELLO
	};

	Hello() : header_{Type} {}

	static const Hello *cast(const Message *message);

	ProtocolVersions protocolVersions() const;

private:
	Header header_;

	bool validateLength(size_t length) const;
	const detail::HelloElement *helloElements() const;

	friend class HelloBuilder;
};

static_assert(sizeof(Hello) == 8, "Unexpected size.");


class HelloBuilder {
public:

	HelloBuilder(ProtocolVersions versions) : bitmap_{versions.bitmap()} {
		msg_.header_.setVersion(versions.highestVersion());
	}

	UInt32 send(Writable *channel);

private:
	Hello msg_;
	detail::HelloElement elem_;
	Big32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_HELLO_H
