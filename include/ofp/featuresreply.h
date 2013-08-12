#ifndef OFP_FEATURESREPLY_H
#define OFP_FEATURESREPLY_H

#include "ofp/header.h"
#include "ofp/padding.h"
#include "ofp/features.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class FeaturesReply {
public:
	enum { Type = OFPT_FEATURES_REPLY };

	FeaturesReply() : header_{Type} {}
	
	static const FeaturesReply *cast(const Message *message);

	void getFeatures(Features *features) const;

private:
	Header header_;
	DatapathID datapathID_;
	Big32 bufferCount_;
	Big8 tableCount_;
	Big8 auxiliaryID_;
	Padding<2> pad_;
	Big32 capabilities_;
	Big32 reserved_;

	bool validateLength(size_t length) const;

	friend class FeaturesReplyBuilder;
};

static_assert(sizeof(FeaturesReply) == 32, "Unexpected size.");

class FeaturesReplyBuilder {
public:
	explicit FeaturesReplyBuilder(const Message *request);

	void setFeatures(const Features &features);
	void send(Channel *channel);

private:
	FeaturesReply msg_;
};

} // </namespace ofp>

#endif // OFP_FEATURESREPLY_H
