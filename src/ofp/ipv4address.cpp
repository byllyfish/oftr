//  ===== ---- ofp/ipv4address.cpp -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements IPv4Address class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/ipv4address.h"
#include "ofp/log.h"
#include <arpa/inet.h>

namespace ofp { // <namespace ofp>

IPv4Address::IPv4Address(const ArrayType &a) : addr_(a)
{
}


IPv4Address::IPv4Address(const std::string &s)
{
	// TODO this function should handle / notation. ie. "/24" should give you a
	// network mask 255.255.255.0. and a.b.c.d/24 should give you a.b.c.0

	if (!parse(s)) {
		addr_.fill(0);
	}
}

/// \returns Subnet mask of the specified length: /24 is 255.255.255.0
IPv4Address IPv4Address::mask(unsigned prefix)
{
	assert(prefix <= 32U);

	IPv4Address result;

	unsigned d = std::min(4U, prefix / 8);
	unsigned r = prefix % 8;

	log::debug("mask", d);

	std::memset(result.addr_.data(), 0xFF, d);
	if (d < 4) {
		result.addr_[d] = UInt8_narrow_cast(0xFFU << (8 - r));
	}

	return result;
}


unsigned IPv4Address::prefix() const
{
	// Count the number of 1 bits in the prefix. Assumes 1 bits are all
	// contiguous.

	unsigned result = 0;

	for (unsigned i = 0; i < Length; ++i) {
		if (addr_[i] != 0xFF) {
			// Count prefix bits in unfilled byte.
			unsigned v = addr_[i];
			while (v & 0x80) {
				v = (v << 1) & 0xFF;
				++result;
			}
			return result;
		}
		result += 8;
	}

	return result;
}


bool IPv4Address::parse(const std::string &s)
{
	int result = inet_pton(AF_INET, s.c_str(), addr_.data());
	return (result > 0);
}


std::string IPv4Address::toString() const
{
	char ipv4str[INET_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET, addr_.data(), ipv4str, sizeof(ipv4str));
	return result ? ipv4str : "<inet_ntop_error4>";
}

} // </namespace ofp>

