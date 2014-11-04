// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_EXPERIMENTER_H_
#define OFP_EXPERIMENTER_H_

#include "ofp/protocolmsg.h"
#include "ofp/bytelist.h"

namespace ofp {

/// \brief Implements immutable Experimenter protocol message.
class Experimenter
    : public ProtocolMsg<Experimenter, OFPT_EXPERIMENTER, 16, 65535, false> {
 public:
  UInt32 experimenter() const { return experimenter_; }

  UInt32 expType() const { return expType_; }

  ByteRange expData() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big32 experimenter_;
  Big32 expType_;

  // Only ExperimenterBuilder can construct an actual instance.
  Experimenter() : header_{type()} {}

  friend class ExperimenterBuilder;
};

static_assert(sizeof(Experimenter) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Experimenter>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Experimenter>(),
              "Expected trivially copyable.");

/// \brief Implements Experimenter protocol message builder.
class ExperimenterBuilder {
 public:
  ExperimenterBuilder() {}

  explicit ExperimenterBuilder(const Experimenter *msg);

  void setExperimenter(UInt32 experimenter) {
    msg_.experimenter_ = experimenter;
  }

  void setExpType(UInt32 expType) { msg_.expType_ = expType; }

  void setExpData(const void *data, size_t length) { data_.set(data, length); }

  UInt32 send(Writable *channel);

 private:
  Experimenter msg_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_EXPERIMENTER_H_
