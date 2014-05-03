#ifndef OFP_TABLEFEATUREPROPERTY_H_
#define OFP_TABLEFEATUREPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/instructionidlist.h"
#include "ofp/actionidlist.h"
#include "ofp/oxmidlist.h"

namespace ofp {

namespace detail {

OFP_BEGIN_IGNORE_PADDING

template <class RangeType, OFPTableFeatureProperty Type>
class TableFeatureProperty {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeatureProperty(RangeType range) : range_{range} {}

    using ValueType = RangeType;
    ValueType value() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }
    static ValueType defaultValue() { return {}; }

private:
    Big16 type_ = type();
    Big16 length_;
    RangeType range_;

    enum : size_t { FixedHeaderSize = 4U };
};

OFP_END_IGNORE_PADDING

}  // namespace detail


using TableFeaturePropertyInstructions = detail::TableFeatureProperty<InstructionIDRange, OFPTFPT_INSTRUCTIONS>;
using TableFeaturePropertyInstructionsMiss = detail::TableFeatureProperty<InstructionIDRange, OFPTFPT_INSTRUCTIONS_MISS>;

using TableFeaturePropertyNextTables = detail::TableFeatureProperty<ByteRange, OFPTFPT_NEXT_TABLES>;
using TableFeaturePropertyNextTablesMiss = detail::TableFeatureProperty<ByteRange, OFPTFPT_NEXT_TABLES_MISS>;

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


class TableFeaturePropertyExperimenter {
public:
    TableFeaturePropertyExperimenter(OFPTableFeatureProperty type);

    UInt32 experimenter() const { return experimenterId_; }
    
private:
    Big16 type_;
    Big16 length_;
    Big32 experimenterId_;
    Big32 expType_;
    ByteList expData_;
};

}  // namespace ofp

#endif // OFP_TABLEFEATUREPROPERTY_H_
