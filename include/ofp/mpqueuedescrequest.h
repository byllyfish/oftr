#ifndef OFP_MPQUEUEDESCREQUEST_H_
#define OFP_MPQUEUEDESCREQUEST_H_

#include "ofp/portnumber.h"
#include "ofp/queuenumber.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPQueueDescRequest {
public:
    static const MPQueueDescRequest *cast(const MultipartRequest *req);

    PortNumber portNo() const { return portNo_; }
    QueueNumber queueId() const { return queueId_; }

    bool validateInput(Validation *context) const;

private:
    PortNumber portNo_;
    QueueNumber queueId_;

    friend class MPQueueDescRequestBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPQueueDescRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPQueueDescRequest>(), "Expected standard layout.");

class MPQueueDescRequestBuilder {
public:
    void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
    void setQueueId(QueueNumber queueId) { msg_.queueId_ = queueId; }

    void write(Writable *channel);

private:
    MPQueueDescRequest msg_;

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_MPQUEUEDESCREQUEST_H_
