#ifndef OFP_MULTIPARTBODY_H
#define OFP_MULTIPARTBODY_H

#include "ofp/bytelist.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class MultipartBody {
public:


private:
	
};


template <class ValueType>
class MultipartBodyBuilder {
public:

	void add(const ValueType &value) { body_.add(&value, sizeof(ValueType)); }

private:
	ByteList body_;
};

} // </namespace ofp>

#endif // OFP_MULTIPARTBODY_H
