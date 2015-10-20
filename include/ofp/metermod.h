// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_METERMOD_H_
#define OFP_METERMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/meterbandlist.h"
#include "ofp/meternumber.h"

namespace ofp {

class MeterMod : public ProtocolMsg<MeterMod, OFPT_METER_MOD, 16> {
 public:
  OFPMeterModCommand command() const { return command_; }
  OFPMeterConfigFlags flags() const { return flags_; }
  MeterNumber meterId() const { return meterId_; }
  MeterBandRange meterBands() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big<OFPMeterModCommand> command_;
  Big<OFPMeterConfigFlags> flags_;
  MeterNumber meterId_;

  // Only MeterModBuilder can construct an instance.
  MeterMod() : header_{type()} {}

  friend class MeterModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MeterMod) == 16, "Unexpected size.");
static_assert(IsStandardLayout<MeterMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<MeterMod>(), "Expected trivially copyable.");

class MeterModBuilder {
 public:
  MeterModBuilder() = default;

  void setCommand(OFPMeterModCommand command) { msg_.command_ = command; }
  void setFlags(OFPMeterConfigFlags flags) { msg_.flags_ = flags; }
  void setMeterId(MeterNumber meterId) { msg_.meterId_ = meterId; }
  void setMeterBands(const MeterBandRange &meterBands) {
    meterBands_.assign(meterBands);
  }

  UInt32 send(Writable *channel);

 private:
  MeterMod msg_;
  MeterBandList meterBands_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_METERMOD_H_
