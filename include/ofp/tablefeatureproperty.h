#ifndef OFP_TABLEFEATUREPROPERTY_H_
#define OFP_TABLEFEATUREPROPERTY_H_

#include "ofp/instructionidlist.h"

namespace ofp {

class TableFeaturePropertyInstructions {
public:
    TableFeaturePropertyInstructions(OFPTableFeatureProperty type);

private:
    Big16 type_;
    Big16 length_;
    InstructionIDList instructionIds;
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
