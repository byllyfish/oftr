#ifndef OFP_OXMFULLTYPE_H_
#define OFP_OXMFULLTYPE_H_

#include "ofp/oxmtype.h"
#include "ofp/oxmfields.h"

namespace ofp {

class OXMFullType {
public:
    OXMFullType() = default;
    OXMFullType(OXMType type, Big32 experimenter) : type_{type}, experimenter_{experimenter}, id_{type.internalID()} {}

    OXMType type() const { return type_; }
    UInt32 experimenter() const { return experimenter_; }
    OXMInternalID internalID() const { return id_; }
    bool hasMask() const { return type_.hasMask(); }
    size_t length() const { return type_.length(); }

    UInt32 oxmNative() const { return type_.oxmNative(); }
    void setOxmNative(UInt32 value) { type_.setOxmNative(value); }

    const OXMTypeInfo *lookupInfo() const { return type_.lookupInfo(); }

    bool parse(const std::string &s);

private:
    OXMType type_;
    Big32 experimenter_;
    OXMInternalID id_ = OXMInternalID::UNKNOWN;
};

inline std::ostream &operator<<(std::ostream &os, const OXMFullType &type) {
    return os << "[OXMFullType " << type.type() << "," << type.experimenter() << "," << static_cast<int>(type.internalID()) << "]";
}

}  // namespace ofp

#endif // OFP_OXMFULLTYPE_H_
