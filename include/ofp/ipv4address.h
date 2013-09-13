//  ===== ---- ofp/ipv4address.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the IPv4Address class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_IPV4ADDRESS_H
#define OFP_IPV4ADDRESS_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class IPv4Address {
public:
	enum { Length = 4 };
	
	using ArrayType = std::array<UInt8,Length>;

	IPv4Address() : addr_{} {}
	explicit IPv4Address(const ArrayType &a);
	explicit IPv4Address(const std::string &s);

	static IPv4Address mask(unsigned prefix);

	bool valid() const {
		return !IsMemFilled(addr_.data(), sizeof(addr_), '\0');
	}

	unsigned prefix() const;
	bool isBroadcast() const { return IsMemFilled(addr_.data(), sizeof(addr_), '\xff'); }
	
	void setAllOnes() {
		std::memset(addr_.data(), 0xFF, sizeof(addr_));
	}

	bool parse(const std::string &s);

	std::string toString() const;

	ArrayType toArray() const {
		return addr_;
	}

	bool operator==(const IPv4Address &rhs) const {
		return addr_ == rhs.addr_;
	}
	
	bool operator!=(const IPv4Address &rhs) const {
		return !(*this == rhs);
	}

	bool fromString(const std::string &s);

private:
	ArrayType addr_;
};

} // </namespace ofp>

#endif //OFP_IPV4ADDRESS_H
