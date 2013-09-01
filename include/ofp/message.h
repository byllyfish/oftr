//  ===== ---- ofp/message.h -------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Message class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MESSAGE_H
#define OFP_MESSAGE_H

#include "ofp/bytelist.h"
#include "ofp/header.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

class Writable;
class Channel;

namespace sys { // <namespace sys>
class Connection;
} // </namespace sys>

/// \brief Implements a protocol message buffer.
class Message {
public:

	Message(sys::Connection *channel) : channel_{channel} {
		buf_.resize(sizeof(Header));
	}

	UInt8 *mutableData(size_t size) { 
		buf_.resize(size);
		return buf_.mutableData(); 
	}

	void shrink(size_t size) {
		assert(size <= buf_.size());
		buf_.resize(size);
	}

	const Header *header() const { return reinterpret_cast<const Header *>(buf_.data()); }
	Header *header() { return reinterpret_cast<Header *>(buf_.mutableData()); }

	const UInt8 *data() const { return buf_.data(); }
	size_t size() const { return buf_.size(); }

	UInt8 type() const { return header()->type(); }
	void setType(UInt8 type) { header()->setType(type); }

	Channel *source() const;
	UInt32 xid() const { return header()->xid(); }


	// Provides convenient implementation of message cast.
	template <class MsgType>
	const MsgType *cast() const 
	{
		assert(type() == MsgType::Type);
		
		const MsgType *msg = reinterpret_cast<const MsgType *>(data());
    	if (!msg->validateLength(size())) {
        	return nullptr;
    	}

    	return msg;
	}

	void transmogrify();

private:
	ByteList buf_;
	sys::Connection *channel_;

	friend std::ostream &operator<<(std::ostream &os, const Message &msg);
};


std::ostream &operator<<(std::ostream &os, const Message &msg);

} // </namespace ofp>


inline std::ostream &ofp::operator<<(std::ostream &os, const Message &msg)
{
	return os << msg.buf_;
}

#endif // OFP_MESSAGE_H
