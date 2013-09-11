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
    static constexpr OFPType type() { return OFPT_FEATURES_REPLY; }
    static const FeaturesReply *cast(const Message *message);

    FeaturesReply() : header_{type()} {}

    void getFeatures(Features *features) const;

    const DatapathID &datapathId() const { return datapathId_; }
    UInt32 bufferCount() const { return bufferCount_; }
    UInt8 tableCount() const { return tableCount_; }
    UInt8 auxiliaryId() const { return auxiliaryId_; }
    UInt32 capabilities() const { return capabilities_; }
    UInt32 reserved() const { return reserved_; }

    //PortRange ports() const;

private:
    Header header_;
    DatapathID datapathId_;
    Big32 bufferCount_;
    Big8 tableCount_;
    Big8 auxiliaryId_;
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
    explicit FeaturesReplyBuilder(UInt32 xid);

    void setFeatures(const Features &features);
    
    void setDatapathId(const DatapathID &datapathId) { msg_.datapathId_ = datapathId; }
    void setBufferCount(UInt32 bufferCount) { msg_.bufferCount_ = bufferCount; }
    void setTableCount(UInt8 tableCount) { msg_.tableCount_ = tableCount; }
    void setAuxiliaryId(UInt8 auxiliaryId) { msg_.auxiliaryId_ = auxiliaryId; }
    void setCapabilities(UInt32 capabilities) { msg_.capabilities_ = capabilities; }
    void setReserved(UInt32 reserved) { msg_.reserved_ = reserved; }
    
    UInt32 send(Writable *channel);

private:
    FeaturesReply msg_;
};

} // </namespace ofp>

#endif // OFP_FEATURESREPLY_H
