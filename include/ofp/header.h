#ifndef OFP_HEADER_H
#define OFP_HEADER_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

// FIXME: strong typing
enum : UInt8 {
	OFPT_HELLO 				= 0,  	// Symmetric message
	OFPT_ERROR 				= 1,  	// Symmetric message
	OFPT_ECHO_REQUEST 		= 2,  	// Symmetric message
	OFPT_ECHO_REPLY 		= 3,  	// Symmetric message
	OFPT_EXPERIMENTER 		= 4,  	// Symmetric message

	// Switch configuration messages.
	OFPT_FEATURES_REQUEST   = 5,  	// Controller/switch message
	OFPT_FEATURES_REPLY     = 6,  	// Controller/switch message
	OFPT_GET_CONFIG_REQUEST = 7,  	// Controller/switch message
	OFPT_GET_CONFIG_REPLY   = 8,  	// Controller/switch message
	OFPT_SET_CONFIG         = 9,  	// Controller/switch message

	// Asynchronous messages.
	OFPT_PACKET_IN          = 10, 	// Async message 
	OFPT_FLOW_REMOVED       = 11, 	// Async message 
	OFPT_PORT_STATUS        = 12, 	// Async message

	// Controller command messages.
	OFPT_PACKET_OUT 		= 13, 	// Controller/switch message 
	OFPT_FLOW_MOD 			= 14, 	// Controller/switch message 
	OFPT_GROUP_MOD 			= 15, 	// Controller/switch message 
	OFPT_PORT_MOD 			= 16, 	// Controller/switch message 
	OFPT_TABLE_MOD 			= 17, 	// Controller/switch message 

	// Multipart messages.
	OFPT_MULTIPART_REQUEST 	= 18, 	// Controller/switch message 
	OFPT_MULTIPART_REPLY 	= 19, 	// Controller/switch message 

	// Barrier messages.
	OFPT_BARRIER_REQUEST 	= 20, 	// Controller/switch message 
	OFPT_BARRIER_REPLY 		= 21, 	// Controller/switch message 

	// Queue Configuration messages.
	OFPT_QUEUE_GET_CONFIG_REQUEST = 22,  // Controller/switch message 
	OFPT_QUEUE_GET_CONFIG_REPLY   = 23,  // Controller/switch message 

	// Controller role change request messages.
	OFPT_ROLE_REQUEST       = 24, 	// Controller/switch message 
	OFPT_ROLE_REPLY         = 25, 	// Controller/switch message 

	// Asynchronous message configuration.
	OFPT_GET_ASYNC_REQUEST  = 26, 	// Controller/switch message 
	OFPT_GET_ASYNC_REPLY    = 27, 	// Controller/switch message 
	OFPT_SET_ASYNC          = 28, 	// Controller/switch message 

	// Meters and rate limiters configuration messages.
	OFPT_METER_MOD          = 29, 	// Controller/switch message 
};

class Header {
public:
	Header() = default;

private:
	Big8 version;			// OFP_VERSION.
	Big8 type;				// One of the OFPT_ constants.
	Big16 length;			// Length including this ofp_header.
	Big32 xid;				// Transaction id for this packet.
};

} // </namespace ofp>

#endif // OFP_HEADER_H
