#ifndef OFP_ACTIONFULLTYPE_H_
#define OFP_ACTIONFULLTYPE_H_

#include "ofp/actiontype.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

/// Concrete class that fully specifies an OpenFlow action. The experimenter
/// and subtype fields are only used if the action is OFPAT_EXPERIMENTER.

class ActionFullType {
public:
    ActionFullType() = default;
    explicit ActionFullType(ActionType type) : type_{type} {}
    ActionFullType(ActionType type, UInt32 experimenter, UInt16 subtype) : type_{type}, experimenter_{experimenter}, subtype_{subtype} {
    }

    ActionType type() const { return type_; }
    OFPActionType enumType() const { return type_.enumType(); }
    UInt32 experimenter() const { return experimenter_; }
    UInt16 subtype() const { return subtype_; }

    bool parse(const std::string &s);

    const ActionTypeInfo *lookupInfo() const;

    void setNative(UInt16 enumType, UInt32 experimenter);

private:
    ActionType type_;
    UInt32 experimenter_ = 0;
    UInt16 subtype_ = 0;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif // OFP_ACTIONFULLTYPE_H_
