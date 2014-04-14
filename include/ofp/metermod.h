#ifndef OFP_METERMOD_H_
#define OFP_METERMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/meterbandlist.h"

namespace ofp {

class MeterMod : public ProtocolMsg<MeterMod, OFPT_METER_MOD, 16> {
 public:
  UInt16 command() const { return command_; }
  UInt16 flags() const { return flags_; }
  UInt32 meterId() const { return meterId_; }
  MeterBandRange meterBands() const;
  
  bool validateInput(size_t length) const;

 private:
  Header header_;
  Big16 command_;
  Big16 flags_;
  Big32 meterId_;

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
  
  void setCommand(UInt16 command) { msg_.command_ = command; }
  void setFlags(UInt16 flags) { msg_.flags_ = flags; }
  void setMeterId(UInt32 meterId) { msg_.meterId_ = meterId; }
  void setMeterBands(const MeterBandRange &meterBands) { meterBands_.assign(meterBands); }

  UInt32 send(Writable *channel);

 private:
  MeterMod msg_;
  MeterBandList meterBands_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_METERMOD_H_
