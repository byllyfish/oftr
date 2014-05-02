#ifndef OFP_TABLEFEATUREPROPERTY_H_
#define OFP_TABLEFEATUREPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/instructionidlist.h"
#include "ofp/actionidlist.h"

namespace ofp {

namespace detail {

template <OFPTableFeatureProperty Type>
class TableFeaturePropertyInstructions {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeaturePropertyInstructions(InstructionIDRange instructionIds) : instructionIds_{instructionIds} {}

    using ValueType = InstructionIDRange;
    InstructionIDRange value() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }
    static InstructionIDRange defaultValue() { return {}; }

private:
    Big16 type_ = type();
    Big16 length_;
    InstructionIDRange instructionIds_;

    enum : size_t { FixedHeaderSize = 4U };
};

template <OFPTableFeatureProperty Type>
class TableFeaturePropertyTables {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeaturePropertyTables(ByteRange tableIds) : tableIds_{tableIds} {}

    using ValueType = ByteRange;
    ValueType value() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }
    static ValueType defaultValue() { return {}; }

private:
    Big16 type_ = type();
    Big16 length_;
    ByteRange tableIds_;

    enum : size_t { FixedHeaderSize = 4U };
};


template <OFPTableFeatureProperty Type>
class TableFeaturePropertyActions {
public:
    constexpr static OFPTableFeatureProperty type() { return Type; }

    TableFeaturePropertyActions(ActionIDRange actionIds) : actionIds_{actionIds} {}

    using ValueType = ActionIDRange;
    ActionIDRange value() const { return ByteRange{BytePtr(this) + FixedHeaderSize, length_ - FixedHeaderSize }; }
    static ActionIDRange defaultValue() { return {}; }

private:
    Big16 type_ = type();
    Big16 length_;
    ActionIDRange actionIds_;

    enum : size_t { FixedHeaderSize = 4U };
};

}  // namespace detail


using TableFeaturePropertyInstructions = detail::TableFeaturePropertyInstructions<OFPTFPT_INSTRUCTIONS>;
using TableFeaturePropertyInstructionsMiss = detail::TableFeaturePropertyInstructions<OFPTFPT_INSTRUCTIONS_MISS>;

using TableFeaturePropertyNextTables = detail::TableFeaturePropertyTables<OFPTFPT_NEXT_TABLES>;
using TableFeaturePropertyNextTablesMiss = detail::TableFeaturePropertyTables<OFPTFPT_NEXT_TABLES_MISS>;

using TableFeaturePropertyWriteActions = detail::TableFeaturePropertyActions<OFPTFPT_WRITE_ACTIONS>;
using TableFeaturePropertyWriteActionsMiss = detail::TableFeaturePropertyActions<OFPTFPT_WRITE_ACTIONS_MISS>;

using TableFeaturePropertyApplyActions = detail::TableFeaturePropertyActions<OFPTFPT_APPLY_ACTIONS>;
using TableFeaturePropertyApplyActionsMiss = detail::TableFeaturePropertyActions<OFPTFPT_APPLY_ACTIONS_MISS>;

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

#if 0
class TableFeaturePropertyTables {
public:
    TableFeaturePropertyTables(OFPTableFeatureProperty type);

private:
    Big16 type_;
    Big16 length_;
    TableIDList tableIds;
};


class TableFeaturePropertyActions {
public:
    TableFeaturePropertyActions(OFPTableFeatureProperty type);

private:
    Big16 type_;
    Big16 length_;
    ActionIDList actionIds;
};
#endif //0


}  // namespace ofp

#endif // OFP_TABLEFEATUREPROPERTY_H_
