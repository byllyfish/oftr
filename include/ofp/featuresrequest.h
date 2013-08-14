#ifndef OFP_FEATURESREQUEST_H
#define OFP_FEATURESREQUEST_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

/**
 *  FeaturesRequest is a concrete class for an OFPT_FEATURES_REQUEST message.
 */
class FeaturesRequest {
public:
    static const FeaturesRequest *cast(const Message *message);

    enum {
        Type = OFPT_FEATURES_REQUEST
    };

    FeaturesRequest();

private:
    Header header_;

    bool validateLength(size_t length) const;

    friend class FeaturesRequestBuilder;
};

static_assert(sizeof(FeaturesRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesRequest>(), "Expected standard layout.");

/**
 *  FeaturesRequestBuilder is a concrete class for building an
 *  OFPT_FEATURES_REQUEST message.
 */
class FeaturesRequestBuilder {
public:
    FeaturesRequestBuilder() = default;

    UInt32 send(Writable *channel);

private:
    FeaturesRequest msg_;
};

} // </namespace ofp>

#endif // OFP_FEATURESREQUEST_H
