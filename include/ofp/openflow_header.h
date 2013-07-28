//===- ofp/openflow_header.h ------------------------------------*- C++ -*-===//
//
// Copyright (c) 2013, Bill Fisher. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// OpenFlow protocol header.
//
//===----------------------------------------------------------------------===//

#include "ofp/byteorder.h"

#ifndef OFP_OPENFLOW_HEADER_H
#define OFP_OPENFLOW_HEADER_H

namespace ofp { // <namespace ofp>
namespace spec { // <namespace spec>

/// \brief Each OpenFlow message begins with the OpenFlow header. 
/// \see {7.1}

enum ofp_version : uint8_t {
	OFP_VERSION_1_0 = 0x01,			// 1.0
	OFP_VERSION_1_1 = 0x02,			// 1.1
	OFP_VERSION_1_2 = 0x03,			// 1.2
	OFP_VERSION_1_3 = 0x04,			// 1.3
};

enum ofp_type : uint8_t {
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

struct ofp_header {
	sys::big<ofp_version> version;		// OFP_VERSION.
	sys::big<ofp_type> type;			// One of the OFPT_ constants.
	sys::big16 length;					// Length including this ofp_header.
	sys::big32 xid;						// Transaction id for this packet.
   										// Replies use the same id as was in the
   										// request to facilitate pairing.
};
static_assert(sizeof(ofp_header) == 8, "Incorrect ofp_header size.");

} // </namespace spec>
} // </namespace ofp>


#endif // OFP_OPENFLOW_HEADER_H
