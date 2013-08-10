#ifndef OFP_CHANNEL_H
#define OFP_CHANNEL_H

#include "ofp/types.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

class Channel {
public:

	virtual ~Channel() {}

	virtual DatapathID datapathID() const = 0;
	virtual UInt8 version() const = 0;
	virtual UInt32 nextXid() = 0;

	virtual void write(const void *data, size_t length) = 0;

	// TODO: add a method to register a callback after a timeout.
	
	#if 0
	void write(AuxConnID aux, const void *data, size_t length, bool eom);
	#endif
};

} // </namespace ofp>

#endif // OFP_CHANNEL_H
