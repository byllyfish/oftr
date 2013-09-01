//  ===== ---- ofp/enetaddress.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the EnetAddress class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ENETADDRESS_H
#define OFP_ENETADDRESS_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class EnetAddress {
public:
	enum { Length = 6 };
	
	using ArrayType = std::array<UInt8,Length>;

	EnetAddress() : addr_{} {}
	explicit EnetAddress(const std::string &s);

	bool parse(const std::string &s);
	
	bool valid() const {
		return !IsMemFilled(addr_.data(), sizeof(addr_), '\0');
	}

	void setAllOnes() {
		std::memset(addr_.data(), 0xFF, sizeof(addr_));
	}

	void clear() {
		std::memset(addr_.data(), 0, sizeof(addr_));
	}

	std::string toString() const;

	ArrayType toArray() const {
		return addr_;
	}

	bool operator==(const EnetAddress &rhs) const {
		return addr_ == rhs.addr_;
	}
	
	bool operator!=(const EnetAddress &rhs) const {
		return !(*this == rhs);
	}
	
private:
	ArrayType  addr_;
};


} // </namespace ofp>

#endif //OFP_ENETADDRESS_H
