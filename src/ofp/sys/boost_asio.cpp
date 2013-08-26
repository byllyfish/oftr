#include "ofp/sys/boost_asio.h"


using namespace boost::asio;


size_t ofp::sys::HashEndpoint::operator()(const udp::endpoint &endpt) const
{
	// FIXME - check literature for better hash function.
	
	// Hash the endpoint starting with the port and the least signficant bytes
	// of the IP address. 
	
	ip::address addr = endpt.address();
	UInt16 port = endpt.port();

	if (addr.is_v6()) {
		auto data = addr.to_v6().to_bytes();

		UInt32 *quad = reinterpret_cast<UInt32*>(&data[0]) + 3;
		UInt64 sum = *quad * (port + 37);
		for (int i = 0; i < 3; ++i)
			sum += 41*sum + (*quad-- + 37);
		sum += 41*sum + (*quad + 37);
		return sum;

	} else {
		assert(addr.is_v4());
		auto data = addr.to_v4().to_bytes();

		UInt32 *quad = reinterpret_cast<UInt32*>(&data[0]);
		UInt64 sum = *quad * (port + 37);
		sum += 41*sum + (*quad + 37);
		return sum;
	}
}