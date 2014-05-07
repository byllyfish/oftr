#ifndef OFP_MPGROUPSTATS_H_
#define OFP_MPGROUPSTATS_H_

#include "ofp/packetcounterlist.h"

namespace ofp {

class Writable;

class MPGroupStats {
public:
    enum { MPReplyVariableSizeOffset = 0 };

    UInt32 groupId() const { return groupId_; }
    UInt32 refCount() const { return refCount_; }
    UInt64 packetCount() const { return packetCount_; }
    UInt64 byteCount() const { return byteCount_; }
    UInt32 durationSec() const { return durationSec_; }
    UInt32 durationNSec() const { return durationNSec_; }
    
    PacketCounterRange bucketStats() const;

    bool validateInput(size_t length) const;

private:
    Big16 length_;
    Padding<2> pad1_;
    Big32 groupId_;
    Big32 refCount_;
    Padding<4> pad2_;
    Big64 packetCount_;
    Big64 byteCount_;
    Big32 durationSec_;
    Big32 durationNSec_;

    friend class MPGroupStatsBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupStats) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupStats>(), "Expected standard layout.");

class MPGroupStatsBuilder {
public:
    MPGroupStatsBuilder() = default;

    void write(Writable *channel);
    void reset() { bucketStats_.clear(); }

private:
    MPGroupStats msg_;
    PacketCounterList bucketStats_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPGROUPSTATS_H_
