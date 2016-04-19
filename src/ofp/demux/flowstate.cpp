#include "ofp/demux/flowstate.h"

using namespace ofp;
using namespace ofp::demux;

static std::string segmentStr(UInt32 begin, UInt32 end) {
    std::ostringstream oss;
    oss << "[" << begin << "," << end << ")";
    return oss.str();
}

FlowData FlowState::receive(const Timestamp &ts, UInt32 end, const ByteRange &data, UInt64 sessionID) {
    UInt32 begin = end - data.size();

    if (!first_.valid()) {
        first_ = ts;
        end_ = begin;
        log::debug("FlowState:", end_, "new flow segment", segmentStr(begin, end));
    } else {
        log::debug("FlowState:", end_, "receive segment", segmentStr(begin, end));
    }

    if (data.empty()) {
        return FlowData{};
    }

    if (cache_.empty()) {
        // We have no data cached. Is this the latest data? If so, return
        // a FlowData object representing new data.
        if (begin == end_) {
            return FlowData{this, data, sessionID, false};
        }
    }

    if (Segment::lessThan(begin, end_)) {
        log::warning("FlowState:", end_, "drop early segment", segmentStr(begin, end));
        return FlowData{};
    }

    // Cache the data segment.
    cache_.store(end, data);

    const Segment *seg = cache_.current();
    if (seg && seg->begin() == end_) {
        log::debug("FlowState:", end_, "return flow data", segmentStr(seg->begin(), seg->end()));
        return FlowData{this, seg->data(), sessionID, true};
    }

    log::warning("Flowstate:", end_, "current cached begin", begin, "does not match flow end");

    return FlowData{};
}


void FlowData::consume(size_t len) {
    // Do nothing if there's no data to consume, or consume() has already been
    // called.
    if (state_ == nullptr) {
        assert(len == 0);
        return;
    }

    assert(state_ != nullptr);
    assert(len <= data_.size());

    state_->end_ += len;

    if (cached_) {
        state_->cache_.consume(len);
    } else if (len < data_.size()) {
        ByteRange left{data_.data() + len, data_.size() - len};
        state_->cache_.store(state_->end_ + left.size(), left);
    }

    state_ = nullptr;
}
