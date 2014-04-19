#ifndef OFP_MPMETERCONFIG_H_
#define OFP_MPMETERCONFIG_H_

#include "ofp/meterbandlist.h"

namespace ofp {

class Writable;

class MPMeterConfig {
public:
    
    UInt16 flags() const { return flags_; }
    UInt32 meterId() const { return meterId_; }

    MeterBandRange bands() const;
    
private:
    Big16 length_;
    Big16 flags_;
    Big32 meterId_;

    friend class MPMeterConfigBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPMeterConfig) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPMeterConfig>(), "Expected standard layout.");

class MPMeterConfigBuilder {
public:

    void setFlags(UInt16 flags) { msg_.flags_ = flags; }
    void setMeterId(UInt32 meterId) { msg_.meterId_ = meterId; }

    void write(Writable *channel);
    void reset() { bands_.clear(); }

private:
    MPMeterConfig msg_;
    MeterBandList bands_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPMETERCONFIG_H_
