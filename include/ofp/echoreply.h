#ifndef OFP_ECHOREPLY_H
#define OFP_ECHOREPLY_H

#include "ofp/header.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class EchoReply {
public:
	enum { Type = OFPT_ECHO_REPLY };

	const EchoReply *cast(const Message *message);
	
	EchoReply() : header_{Type} {}

private:
	Header header_;
};


class EchoReplyBuilder {
public:
	explicit EchoReplyBuilder(const EchoRequest *request);

	void send(Writable *channel);

private:
	EchoReply msg_;
};

} // </namespace ofp>

#endif // OFP_ECHOREPLY_H
