#ifndef OFP_HEADER_H
#define OFP_HEADER_H

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class Header {
public:
	Header(UInt8 type) : type_{type} {}

	UInt8 version() const;
	void setVersion(UInt8 version);

	UInt8 type() const;
	void setType(UInt8 type);

	UInt16 length() const;
	void setLength(UInt16 length);

	UInt32 xid() const;
	void setXid(UInt32 xid);

private:
	Big8 version_ = 0;			// OFP_VERSION.
	Big8 type_;					// One of the OFPT_ constants.
	Big16 length_ = 0;			// Length including this ofp_header.
	Big32 xid_ = 0;				// Transaction id for this packet.
};

static_assert(sizeof(Header) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Header>(), "Expected standard layout.");


} // </namespace ofp>


inline ofp::UInt8 ofp::Header::version() const
{
	return version_;
}

inline ofp::UInt8 ofp::Header::type() const
{
	return type_;
}

inline ofp::UInt16 ofp::Header::length() const
{
	return length_;
}

inline ofp::UInt32 ofp::Header::xid() const
{
	return xid_;
}

inline void ofp::Header::setVersion(UInt8 version) {
	version_ = version;
}
inline void ofp::Header::setType(UInt8 type) {
	type_ = type;
}
inline void ofp::Header::setLength(UInt16 length) {
	length_ = length;
}
inline void ofp::Header::setXid(UInt32 xid) {
	xid_ = xid;
}

#endif // OFP_HEADER_H
