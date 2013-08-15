#ifndef OFP_HEADER_H
#define OFP_HEADER_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

enum : UInt8 {
	VERSION_1_0 = 0x01,
	VERSION_1_1 = 0x02,
	VERSION_1_2 = 0x03,
	VERSION_1_3 = 0x04,
};

// FIXME: strong typing?
enum : UInt8 {
	OFPT_HELLO 				= 0,  	// Symmetric message
	OFPT_ERROR 				= 1,  	// Symmetric message
	OFPT_ECHO_REQUEST 		= 2,  	// Symmetric message
	OFPT_ECHO_REPLY 		= 3,  	// Symmetric message
	OFPT_EXPERIMENTER 		= 4,  	// Symmetric message

	// Switch configuration messages.
	OFPT_FEATURES_REQUEST   = 5,  	// Controller -> switch message
	OFPT_FEATURES_REPLY     = 6,  	// Controller -> switch message
	OFPT_GET_CONFIG_REQUEST = 7,  	// Controller -> switch message
	OFPT_GET_CONFIG_REPLY   = 8,  	// Controller -> switch message
	OFPT_SET_CONFIG         = 9,  	// Controller -> switch message

	// Asynchronous messages.
	OFPT_PACKET_IN          = 10, 	// Switch -> controller message
	OFPT_FLOW_REMOVED       = 11, 	// Switch -> controller message
	OFPT_PORT_STATUS        = 12, 	// Switch -> controller message

	// Controller command messages.
	OFPT_PACKET_OUT 		= 13, 	// Controller -> switch message 
	OFPT_FLOW_MOD 			= 14, 	// Controller -> switch message 
	OFPT_GROUP_MOD 			= 15, 	// Controller -> switch message 
	OFPT_PORT_MOD 			= 16, 	// Controller -> switch message 
	OFPT_TABLE_MOD 			= 17, 	// Controller -> switch message 

	// Multipart messages.
	OFPT_MULTIPART_REQUEST 	= 18, 	// Controller -> switch message 
	OFPT_MULTIPART_REPLY 	= 19, 	// Controller -> switch message 

	// Barrier messages.
	OFPT_BARRIER_REQUEST 	= 20, 	// Controller -> switch message 
	OFPT_BARRIER_REPLY 		= 21, 	// Controller -> switch message 

	// Queue Configuration messages.
	OFPT_QUEUE_GET_CONFIG_REQUEST = 22,  // Controller -> switch message 
	OFPT_QUEUE_GET_CONFIG_REPLY   = 23,  // Controller -> switch message 

	// Controller role change request messages.
	OFPT_ROLE_REQUEST       = 24, 	// Controller -> switch message 
	OFPT_ROLE_REPLY         = 25, 	// Controller -> switch message 

	// Asynchronous message configuration.
	OFPT_GET_ASYNC_REQUEST  = 26, 	// Controller -> switch message 
	OFPT_GET_ASYNC_REPLY    = 27, 	// Controller -> switch message 
	OFPT_SET_ASYNC          = 28, 	// Controller -> switch message

	// Meters and rate limiters configuration messages.
	OFPT_METER_MOD          = 29, 	// Controller -> switch message
};

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
