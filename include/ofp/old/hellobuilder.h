#ifndef OFP_HELLOBUILDER_H
#define OFP_HELLOBUILDER_H

#include "ofp/hello.h"

namespace ofp { // <namespace ofp>

class HelloBuilder {
public:

	HelloBuilder(ProtocolVersions versions) : bitmap_{versions.bitmap()} {
		msg_.header_.version = versions.highestVersion();
	}

	UInt32 send(Channel *channel);

private:
	Hello msg_;
	detail::HelloElement elem_;
	Big32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_HELLOBUILDER_H
