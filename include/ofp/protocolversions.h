#ifndef OFP_PROTOCOL_VERSIONS_H
#define OFP_PROTOCOL_VERSIONS_H

#include "ofp/types.h"
#include <initializer_list>

namespace ofp { // <namespace ofp>

class ProtocolVersions {
public:

	enum {
		MaxVersion = 0x04
	};

	enum Setting : UInt32 { 
		All = ~(~0U << (MaxVersion+1)) & ~1U,
		None = 0U
	};

 	ProtocolVersions();
	ProtocolVersions(Setting setting);
	ProtocolVersions(std::initializer_list<UInt8> versions);

	bool empty() const;
	UInt8 highestVersion() const;
	UInt32 bitmap() const;

	ProtocolVersions intersection(ProtocolVersions versions) const;

	static ProtocolVersions fromBitmap(UInt32 bitmap);

private:
	UInt32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_PROTOCOL_VERSIONS_H
