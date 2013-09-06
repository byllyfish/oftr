//  ===== ---- ofp/exception.h -----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines a common class for representing error codes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_EXCEPTION_H
#define OFP_EXCEPTION_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class Channel;

/// \brief Represents an error code in the system.
/// This class is named `Exception` because the word `Error` is used to refer to
/// a specific message in the OpenFlow protocol. Exception objects may be 
/// returned from functions, but they will never be thrown; this library is 
/// designed to be compiled with exceptions disabled.
class Exception {
public:
	/// \brief Represents the error category with a 4-char code.
	using Category = std::array<char,4>;

	Exception() : category_{}, code_{} {}
	
	explicit Exception(Category category, int code) : code_{code} {
		category_ = category;
	}

	operator bool() const {
		return code_ != 0;
	}

	std::string toString() const {
		return std::string{category_.begin(), category_.end()} + '|' + std::to_string(code_);
	}

private:
	Category category_;
	int code_;
};

std::ostream &operator<<(std::ostream &os, const Exception &ex);

inline std::ostream &operator<<(std::ostream &os, const Exception &ex)
{
	return os << ex.toString();
}

} // </namespace ofp>

#endif // OFP_EXCEPTION_H
