#ifndef OFP_EXCEPTION_H
#define OFP_EXCEPTION_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class Channel;

/**
 *  0 means no exception.
 */
class Exception {
public:
	using Category = std::array<char,4>;

	Exception() : category_{}, code_{}, channel_{nullptr} {}
	
	explicit Exception(Category category, int code, Channel *channel = nullptr) : code_{code}, channel_{channel} {
		category_ = category;
	}

	std::string toString() const {
		return std::string{category_.begin(), category_.end()} + '|' + std::to_string(code_);
	}

private:
	Category category_;
	int code_;
	Channel *channel_;
};

std::ostream &operator<<(std::ostream &os, const Exception &ex);

} // </namespace ofp>



inline std::ostream &ofp::operator<<(std::ostream &os, const Exception &ex)
{
	return os << ex.toString();
}


#endif // OFP_EXCEPTION_H
