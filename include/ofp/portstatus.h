

#ifndef OFP_PORTSTATUS_H
#define OFP_PORTSTATUS_H

#include "ofp/port.h"

namespace ofp { // <namespace ofp>

class PortStatus {
public:
	enum { Type = OFPT_PORT_STATUS };

	static const PortStatus *cast(const Message *message);

	PortStatus() : header_{Type} {}

private:
	Header header_;
	UInt8 reason_;
	Padding<7> pad_;
	Port port_;

	bool validateLength(size_t length) const;

	friend class PortStatusBuilder;
};

static_assert(sizeof(PortStatus) == 80, "Unexpected size.");
static_assert(IsStandardLayout<PortStatus>(), "Expected standard layout.");

class PortStatusBuilder {
public:
	PortStatusBuilder() = default;

	UInt32 send(Writable *channel);

private:
	PortStatus msg_;
};

} // </namespace ofp>

#endif // OFP_PORTSTATUS_H
