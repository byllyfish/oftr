#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

DatapathID::DatapathID(Big16 implementerDefined, EnetAddress macAddress)
{
    std::memcpy(dpid_.data(), &implementerDefined, sizeof(implementerDefined));
    std::memcpy(dpid_.data() + 2, &macAddress, sizeof(macAddress));
}

Big16 DatapathID::implementerDefined() const
{
    Big16 result;
    std::memcpy(&result, dpid_.data(), sizeof(result));
    return result;
}

EnetAddress DatapathID::macAddress() const
{
    EnetAddress result;
    std::memcpy(&result, dpid_.data() + 2, sizeof(result));
    return result;
}

std::string DatapathID::toString() const
{
	return RawDataToHex(dpid_.data(), sizeof(dpid_), '-', 2);
}


bool DatapathID::parse(const std::string &s)
{
	return HexToRawData(s, dpid_.data(), sizeof(dpid_)) >= sizeof(dpid_);
}

} // </namespace ofp>