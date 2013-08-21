#include "ofp/enetaddress.h"


ofp::EnetAddress::EnetAddress(const std::string &s)
{
	if (!parse(s)) {
		std::memset(addr_, 0, sizeof(addr_));
	}
}


bool ofp::EnetAddress::parse(const std::string &s)
{
	return HexToRawData(s, addr_, sizeof(addr_)) >= sizeof(addr_);
}


std::string ofp::EnetAddress::toString() const
{
	return RawDataToHex(addr_, sizeof(addr_), '-', 1);
}

