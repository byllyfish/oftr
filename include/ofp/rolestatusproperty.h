// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_ROLESTATUSPROPERTY_H_
#define OFP_ROLESTATUSPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/propertyrange.h"
#include "ofp/byteorder.h"

namespace ofp {

class RoleStatusPropertyExperimenter {
 public:
  constexpr static OFPRoleStatusProperty type() { return OFPRPT_EXPERIMENTER; }

  enum { FixedHeaderSize = 12 };

  RoleStatusPropertyExperimenter(UInt32 experimenterId, UInt32 expType,
                                 const ByteRange &data)
      : len_(UInt16_narrow_cast(FixedHeaderSize + data.size())),
        experimenter_{experimenterId},
        expType_{expType},
        data_{data} {}

  UInt32 size() const { return len_; }
  UInt32 experimenter() const { return experimenter_; }
  UInt32 expType() const { return expType_; }

  ByteRange expData() const {
    return ByteRange{BytePtr(this) + FixedHeaderSize, size() - FixedHeaderSize};
  }

  ByteRange valueRef() const { return data_; }

 private:
  Big16 type_ = type();
  Big16 len_;
  Big32 experimenter_;
  Big32 expType_;
  ByteRange data_;

  friend class PropertyList;
};

class RoleStatusPropertyValidator {
 public:
  static bool validateInput(const PropertyRange &range, Validation *context);
};

}  // namespace ofp

#endif  // OFP_ROLESTATUSPROPERTY_H_
