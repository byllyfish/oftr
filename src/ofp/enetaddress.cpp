#include "ofp/enetaddress.h"


ofp::EnetAddress::EnetAddress(const std::string &s)
{
	if (!parse(s)) {
		clear();
	}
}


bool ofp::EnetAddress::parse(const std::string &s)
{
	return HexToRawData(s, addr_.data(), sizeof(addr_)) >= sizeof(addr_);
}


std::string ofp::EnetAddress::toString() const
{
	return RawDataToHex(addr_.data(), sizeof(addr_), '-', 1);
}

