#ifndef OFP_TABLEFEATUREPROPERTY_H_
#define OFP_TABLEFEATUREPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/instructionidlist.h"
#include "ofp/actionidlist.h"
#include "ofp/oxmidlist.h"
#include "ofp/tableidlist.h"

namespace ofp {

class PropertyRange;

namespace detail {

OFP_BEGIN_IGNORE_PADDING

template <class RangeType, OFPTableFeatureProperty Type>
class TableFeatureProperty {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeatureProperty(RangeType range) : length_{UInt16_narrow_cast(range.size() + FixedHeaderSize)}, range_{range} {}

    size_t size() const { return length_; }

    using ValueType = RangeType;
    ValueType value() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }
    static ValueType defaultValue() { return {}; }

    enum : size_t { FixedHeaderSize = 4U };

    ByteRange valueRef() const { return range_.toByteRange(); }

    bool validateInput() const { return value().validateInput("TableFeature"); }

private:
    Big16 type_ = type();
    Big16 length_;
    RangeType range_;
};


template <OFPTableFeatureProperty Type>
class TableFeaturePropertyExperimenter {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeaturePropertyExperimenter(UInt32 experimenter, UInt32 expType, ByteRange expData) : length_(UInt16_narrow_cast(expData.size() + FixedHeaderSize)), experimenterId_{experimenter}, expType_{expType}, expData_{expData} {}

    size_t size() const { return length_; }
    UInt32 experimenter() const { return experimenterId_; }
    UInt32 expType() const { return expType_; }
    ByteRange expData() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }

    enum : size_t { FixedHeaderSize = 8U };

    ByteRange valueRef() const { return expData_; }

private:
    Big16 type_ = type();
    Big16 length_;
    Big32 experimenterId_;
    Big32 expType_;
    ByteRange expData_;
};

OFP_END_IGNORE_PADDING

}  // namespace detail


using TableFeaturePropertyInstructions = detail::TableFeatureProperty<InstructionIDRange, OFPTFPT_INSTRUCTIONS>;
using TableFeaturePropertyInstructionsMiss = detail::TableFeatureProperty<InstructionIDRange, OFPTFPT_INSTRUCTIONS_MISS>;

using TableFeaturePropertyNextTables = detail::TableFeatureProperty<TableIDRange, OFPTFPT_NEXT_TABLES>;
using TableFeaturePropertyNextTablesMiss = detail::TableFeatureProperty<TableIDRange, OFPTFPT_NEXT_TABLES_MISS>;

using TableFeaturePropertyWriteActions = detail::TableFeatureProperty<ActionIDRange, OFPTFPT_WRITE_ACTIONS>;
using TableFeaturePropertyWriteActionsMiss = detail::TableFeatureProperty<ActionIDRange, OFPTFPT_WRITE_ACTIONS_MISS>;
using TableFeaturePropertyApplyActions = detail::TableFeatureProperty<ActionIDRange, OFPTFPT_APPLY_ACTIONS>;
using TableFeaturePropertyApplyActionsMiss = detail::TableFeatureProperty<ActionIDRange, OFPTFPT_APPLY_ACTIONS_MISS>;

using TableFeaturePropertyMatch = detail::TableFeatureProperty<OXMIDRange, OFPTFPT_MATCH>;
using TableFeaturePropertyWildcards= detail::TableFeatureProperty<OXMIDRange, OFPTFPT_WILDCARDS>;
using TableFeaturePropertyWriteSetField = detail::TableFeatureProperty<OXMIDRange, OFPTFPT_WRITE_SETFIELD>;
using TableFeaturePropertyWriteSetFieldMiss = detail::TableFeatureProperty<OXMIDRange, OFPTFPT_WRITE_SETFIELD_MISS>;
using TableFeaturePropertyApplySetField = detail::TableFeatureProperty<OXMIDRange, OFPTFPT_APPLY_SETFIELD>;
using TableFeaturePropertyApplySetFieldMiss = detail::TableFeatureProperty<OXMIDRange, OFPTFPT_APPLY_SETFIELD_MISS>;

using TableFeaturePropertyExperimenter = detail::TableFeaturePropertyExperimenter<OFPTFPT_EXPERIMENTER>;
using TableFeaturePropertyExperimenterMiss = detail::TableFeaturePropertyExperimenter<OFPTFPT_EXPERIMENTER_MISS>;


class TableFeaturePropertyValidator {
public:
    static bool validateInput(const PropertyRange &range);
};

}  // namespace ofp

#endif // OFP_TABLEFEATUREPROPERTY_H_
