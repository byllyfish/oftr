#ifndef OFP_MESSAGE_H
#define OFP_MESSAGE_H

#include "ofp/bytelist.h"
#include "ofp/header.h"

namespace ofp { // <namespace ofp>

class Message {
public:

	Message() = default;

	UInt8 *mutableData() { return buf_.mutableData(); }
	const Header *header() const { return reinterpret_cast<const Header *>(buf_.data()); }
	size_t size() const { return buf_.size(); }

	void resize(size_t length) { buf_.resize(length); }

	UInt8 auxiliaryID() const { return auxiliaryID_; }
	void setAuxiliaryID(UInt8 auxiliaryID) { auxiliaryID_ = auxiliaryID; }

private:
	ByteList buf_;
	UInt8 auxiliaryID_ = 0;
};

} // </namespace ofp>

#endif // OFP_MESSAGE_H
