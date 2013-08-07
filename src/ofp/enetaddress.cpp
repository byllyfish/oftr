#include "ofp/enetaddress.h"


ofp::EnetAddress::EnetAddress(const std::string &s)
{
	if (HexToRawData(s, addr_, sizeof(addr_)) < sizeof(addr_)) {
		std::memset(addr_, 0, sizeof(addr_));
	}
}


std::string ofp::EnetAddress::toString() const
{
	return RawDataToHex(addr_, sizeof(addr_), '-', 1);
}

