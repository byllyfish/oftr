#ifndef OFP_ACTIONLIST_H
#define OFP_ACTIONLIST_H

#include "ofp/types.h"
#include <vector>

namespace ofp { // <namespace ofp>

class ActionList {
public:

	ActionList() = default;

	const UInt8 *data() const { return &buf_[0]; }
	size_t size() const { return buf_.size(); }

	template<class Type>
	void add(const Type &action) {
		add(&action, sizeof(action));
	}

private:
	std::vector<UInt8> buf_;

	void add(const void *data, size_t len) 
	{
		const UInt8 *p = static_cast<const UInt8*>(data);
		buf_.insert(buf_.end(), p, p + len);
	}
};

} // </namespace ofp>

#endif // OFP_ACTIONLIST_H
