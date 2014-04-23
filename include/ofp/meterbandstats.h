#ifndef OFP_METERBANDSTATS_H_
#define OFP_METERBANDSTATS_H_

#include "ofp/byteorder.h"

namespace ofp {

class MeterBandStats {
public:
    enum { ProtocolIteratorSizeOffset = 0xffff };

    UInt64 packetBandCount() const { return packetBandCount_; }
    UInt64 byteBandCount() const { return byteBandCount_; }

private:
    Big64 packetBandCount_;
    Big64 byteBandCount_;

    friend class MeterBandStatsBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MeterBandStats) == 16, "Unexpected size.");
static_assert(IsStandardLayout<MeterBandStats>(), "Expected standard layout.");


class MeterBandStatsBuilder {
public:
    MeterBandStatsBuilder() = default;

    void setPacketBandCount(UInt64 packetBandCount) { msg_.packetBandCount_ = packetBandCount; }
    void setByteBandCount(UInt64 byteBandCount) { msg_.byteBandCount_ = byteBandCount; }

private:
    MeterBandStats msg_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_METERBANDSTATS_H_
