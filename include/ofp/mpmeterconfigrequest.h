#ifndef OFP_MPMETERCONFIGREQUEST_H_
#define OFP_MPMETERCONFIGREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Writable;
class MultipartRequest;

class MPMeterConfigRequest {
public:
    static const MPMeterConfigRequest *cast(const MultipartRequest *req);

    UInt32 meterId() const { return meterId_; }

    bool validateInput(size_t length) const { return length == 8; }

private:
    Big32 meterId_;
    Padding<4> pad_;

    friend class MPMeterConfigRequestBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPMeterConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPMeterConfigRequest>(), "Expected standard layout.");

class MPMeterConfigRequestBuilder {
public:
    MPMeterConfigRequestBuilder() = default;

    void setMeterId(UInt32 meterId) { msg_.meterId_ = meterId; }

    void write(Writable *channel);
    void reset() { }

private:
    MPMeterConfigRequest msg_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPMETERCONFIGREQUEST_H_
