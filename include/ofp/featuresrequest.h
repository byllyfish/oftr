#ifndef OFP_FEATURESREQUEST_H
#define OFP_FEATURESREQUEST_H

#include "ofp/header.h"

namespace ofp { // <namespace ofp>

class Channel;

class FeaturesRequest {
public:
	enum { Type = OFPT_FEATURES_REQUEST };

	FeaturesRequest() : header_{Type} {}

private:
	Header header_;

	friend class FeaturesRequestBuilder;
};


class FeaturesRequestBuilder {
public:
	
	void send(Channel *channel);

private:
	FeaturesRequest msg_;
};


} // </namespace ofp>

#endif // OFP_FEATURESREQUEST_H
