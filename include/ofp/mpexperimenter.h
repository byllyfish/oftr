// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPEXPERIMENTER_H_
#define OFP_MPEXPERIMENTER_H_

#include "ofp/bytelist.h"
#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPExperimenter {
 public:
  static const MPExperimenter *cast(const MultipartRequest *req);

  UInt32 experimenter() const { return experimenter_; }
  UInt32 expType() const { return expType_; }
  ByteRange expData() const;

  bool validateInput(Validation *context) const;

 private:
  Big32 experimenter_;
  Big32 expType_;

  // Only a MPExperimenterBuilder can construct a MPExperimenter.
  MPExperimenter() = default;

  friend class MPExperimenterBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPExperimenter) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPExperimenter>(), "Expected standard layout.");

class MPExperimenterBuilder {
 public:
  MPExperimenterBuilder() = default;

  void write(Writable *channel);
  void reset() { data_.clear(); }

 private:
  MPExperimenter msg_;
  ByteList data_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPEXPERIMENTER_H_
