#ifndef OFP_DEMUX_FLOWSTATE_H_
#define OFP_DEMUX_FLOWSTATE_H_

#include "ofp/demux/segmentcache.h"
#include "ofp/timestamp.h"

namespace ofp {
namespace demux {

class FlowState;

class FlowData {
public:
    FlowData() = default;
    FlowData(FlowState *state, const ByteRange &data, UInt64 sessionID, bool cached): state_{state}, data_{data}, sessionID_{sessionID}, cached_{cached} {}
    FlowData(FlowData &&other): state_{other.state_}, data_{other.data_}, sessionID_{other.sessionID_}, cached_{other.cached_} { other.state_ = nullptr; }
    ~FlowData() { assert(state_ == nullptr); }

    const UInt8 *data() const { return data_.data(); }
    size_t size() const { return data_.size(); }

    void consume(size_t len);

    FlowData(const FlowData &data) = delete;
    FlowData &operator=(const FlowData &data) = delete;

private:
    FlowState *state_ = nullptr;
    ByteRange data_;
    UInt64 sessionID_ = 0;
    bool cached_ = false;
};


class FlowState {
public:
    FlowData receive(const Timestamp &ts, UInt32 end, const ByteRange &data, UInt64 sessionID);

    const Timestamp &first() const { return first_; }
    UInt32 end() const { return end_; }
    bool empty() const { return cache_.empty(); }
    
private:
    Timestamp first_;
    UInt32 end_ = 0;
    SegmentCache cache_;

    friend class FlowData;
};

}  // namespace demux
}  // namespace ofp

#endif // OFP_DEMUX_FLOWSTATE_H_
