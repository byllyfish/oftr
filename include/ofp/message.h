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

	explicit Message(sys::Connection *channel) : channel_{channel} {
		buf_.resize(sizeof(Header));
	}

	Message(const void *data, size_t size) : channel_{nullptr}
	{
		buf_.add(data, size);
		assert(header()->length() == size);
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

	OFPType type() const { return header()->type(); }
	void setType(OFPType type) { header()->setType(type); }

	Channel *source() const;
	UInt32 xid() const { return header()->xid(); }
	UInt8 version() const { return header()->version(); }

	// Provides convenient implementation of message cast.
	template <class MsgType>
	const MsgType *cast() const 
	{
		assert(type() == MsgType::type());
		
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

	void transmogrifyFlowModV1();
	void transmogrifyPortStatusV1();
	void transmogrifyExperimenterV1();

#if 0
	int transmogrifyInstructionsV1orV2(const InstructionRange &instr);
	int transmogrifyActionsV1orV2(const ActionRange &actions);
	int transmogrifyActionV1orV2(UInt16 type, ActionIterator *iter, ActionIterator *iterEnd);
#endif //0
#
	friend std::ostream &operator<<(std::ostream &os, const Message &msg);

	friend class Transmogrify;
};


std::ostream &operator<<(std::ostream &os, const Message &msg);

inline std::ostream &operator<<(std::ostream &os, const Message &msg)
{
	return os << msg.buf_;
}

} // </namespace ofp>

#endif // OFP_MESSAGE_H
