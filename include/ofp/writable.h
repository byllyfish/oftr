#ifndef OFP_WRITABLE_H
#define OFP_WRITABLE_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class Writable {
public:
	virtual ~Writable();

	virtual UInt8 version() const = 0;
	virtual UInt32 nextXid() = 0;

	virtual void write(const void *data, size_t length) = 0;
	virtual void flush() = 0;
};

} // </namespace ofp>

#endif // OFP_WRITABLE_H
