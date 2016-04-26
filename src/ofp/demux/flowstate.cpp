// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowstate.h"

using namespace ofp;
using namespace ofp::demux;

static std::string segmentStr(UInt32 begin, UInt32 end) {
  std::ostringstream oss;
  oss << "[" << begin << "," << end << ")";
  return oss.str();
}

void FlowData::consume(size_t len) {
  if (state_ == nullptr) {
    // Do nothing if there's no data to consume.
    if (len > 0) {
      log::debug("FlowData::consume: empty - can't consume", len);
    }
    return;
  }

  assert(state_ != nullptr);
  assert(len <= data_.size());

  state_->end_ += len;

  if (cached_) {
    state_->cache_.consume(len);
  } else if (len < data_.size()) {
    ByteRange left{data_.data() + len, data_.size() - len};
    state_->cache_.store(state_->end_ + UInt32_narrow_cast(left.size()), left);
  }

  state_ = nullptr;
  data_.clear();
}

FlowData FlowState::receive(const Timestamp &ts, UInt32 end,
                            const ByteRange &data, UInt64 sessionID, bool final) {
  UInt32 begin = end - UInt32_narrow_cast(data.size());

  if (!first_.valid()) {
    first_ = ts;
    end_ = begin;
    const char *finalStr = final ? "final" : "";
    log::debug("FlowState: new flow segment", segmentStr(begin, end), finalStr);
  } else {
    const char *finalStr = final ? "final" : "";
    log::debug("FlowState: receive segment", segmentStr(begin, end), finalStr);
  }

  // Record timestamp of latest segment.
  last_ = ts;

  if (cache_.empty() && !finished_) {
    // We have no data cached. Is this the latest data? If so, return
    // a FlowData object representing new data.
    if (begin == end_) {
      if (final) {
        finished_ = true;
      }
      return FlowData{this, data, sessionID, false, final};
    }
  }

  if (Segment::lessThan(begin, end_)) {
    log::warning("FlowState: drop early segment", segmentStr(begin, end));
    return FlowData{sessionID};
  }

  // Cache the data segment.
  cache_.store(end, data, final);

  if (final) {
    finished_ = true;
  }

  return latestData(sessionID);
}

FlowData FlowState::latestData(UInt64 sessionID) {
  const Segment *seg = cache_.current();

  if (seg && seg->begin() == end_) {
    log::debug("FlowState: return flow data", segmentStr(seg->begin(), seg->end()));
    return FlowData{this, seg->data(), sessionID, true, seg->final()};
  }

  return FlowData{sessionID};
}

void FlowState::clear() {
  first_.clear();
  last_.clear();
  end_ = 0;
  finished_ = false;
  cache_.clear();
}
