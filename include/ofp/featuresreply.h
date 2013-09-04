//  ===== ---- ofp/featuresreply.h -------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the FeaturesReply and FeaturesReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FEATURESREPLY_H
#define OFP_FEATURESREPLY_H

#include "ofp/header.h"
#include "ofp/padding.h"
#include "ofp/features.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

/**
 *  FeaturesReply is a concrete class for an OFPT_FEATURES_REPLY message.
 */
class FeaturesReply {
public:
    static const FeaturesReply *cast(const Message *message);

    enum {
        Type = OFPT_FEATURES_REPLY
    };

    FeaturesReply();

    void getFeatures(Features *features) const;
    //PortRange ports() const;

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
static_assert(IsStandardLayout<FeaturesReply>(), "Expected standard layout.");

/**
 *  FeaturesReplyBuilder is a concrete class for building an OFPT_FEATURES_REPLY
 *  message.
 */
class FeaturesReplyBuilder {
public:
    explicit FeaturesReplyBuilder(const Message *request);

    void setFeatures(const Features &features);
    
    UInt32 send(Writable *channel);

private:
    FeaturesReply msg_;
};

} // </namespace ofp>

#endif // OFP_FEATURESREPLY_H
