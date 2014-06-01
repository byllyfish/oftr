#ifndef OFP_MPTABLEFEATURES_H_
#define OFP_MPTABLEFEATURES_H_

#include "ofp/propertylist.h"
#include "ofp/smallcstring.h"

namespace ofp {

class Writable;

using TableName = SmallCString<OFP_MAX_TABLE_NAME_LEN>;

static_assert(sizeof(TableName) == OFP_MAX_TABLE_NAME_LEN, "Unexpected size.");
static_assert(IsStandardLayout<TableName>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<TableName>(), "Expected trivially copyable.");


class MPTableFeatures {
public:
    enum { MPReplyVariableSizeOffset = 0 };

    UInt8 tableId() const { return tableId_; }
    const TableName &name() const { return name_; }
    UInt64 metadataMatch() const { return metadataMatch_; }
    UInt64 metadataWrite() const { return metadataWrite_; }
    UInt32 config() const { return config_; }
    UInt32 maxEntries() const { return maxEntries_; }

    PropertyRange properties() const;

private:
    Big16 length_;
    Big8 tableId_;
    Padding<5> pad_;
    TableName name_;
    Big64 metadataMatch_;
    Big64 metadataWrite_;
    Big32 config_;
    Big32 maxEntries_;

    friend class MPTableFeaturesBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPTableFeatures) == 64, "Unexpected size.");
static_assert(IsStandardLayout<MPTableFeatures>(), "Expected standard layout.");


class MPTableFeaturesBuilder {
public:

    void setProperties(const PropertyList &properties) { properties_ = properties; }

    void write(Writable *channel);
    void reset() { properties_.clear(); }

private:
    MPTableFeatures msg_;
    PropertyList properties_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPTABLEFEATURES_H_
