// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMFULLTYPE_H_
#define OFP_OXMFULLTYPE_H_

#include <vector>
#include "ofp/oxmfields.h"
#include "ofp/oxmtype.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class OXMFullType {
 public:
  OXMFullType() = default;
  OXMFullType(OXMType type, Big32 experimenter)
      : type_{type},
        experimenter_{experimenter},
        id_{type.internalID_Experimenter(experimenter)} {}

  OXMType type() const { return type_; }
  UInt32 experimenter() const { return experimenter_; }
  OXMInternalID internalID() const { return id_; }
  bool hasMask() const { return type_.hasMask(); }
  size_t length() const { return type_.length(); }

  UInt32 oxmNative() const { return type_.oxmNative(); }
  void setOxmNative(UInt32 value, Big32 experimenter) {
    type_.setOxmNative(value);
    experimenter_ = experimenter;
    id_ = type_.internalID_Experimenter(experimenter_);
  }

  const OXMTypeInfo *lookupInfo() const {
    unsigned idx = static_cast<unsigned>(id_);
    return idx < OXMTypeInfoArraySize ? &OXMTypeInfoArray[idx] : nullptr;
  }

  bool parse(llvm::StringRef s);

  static std::vector<llvm::StringRef> listAll();

 private:
  OXMType type_;
  Big32 experimenter_;
  OXMInternalID id_ = OXMInternalID::UNKNOWN;
};

OFP_END_IGNORE_PADDING

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                     const OXMFullType &type) {
  return os << "[OXMFullType " << type.type() << "," << type.experimenter()
            << "," << static_cast<int>(type.internalID()) << "]";
}

}  // namespace ofp

#endif  // OFP_OXMFULLTYPE_H_
