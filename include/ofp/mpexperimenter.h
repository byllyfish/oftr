#ifndef OFP_MPEXPERIMENTER_H_
#define OFP_MPEXPERIMENTER_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp {

class Writable;
class MultipartRequest;

class MPExperimenter {
 public:
  static const MPExperimenter *cast(const MultipartRequest *req);

  UInt32 experimenter() const { return experimenter_; }
  UInt32 expType() const { return expType_; }
  ByteRange expData() const;

  bool validateInput(size_t length) const { return length >= 8; }

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
