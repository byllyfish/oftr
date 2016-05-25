// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowstate.h"

using namespace ofp;
using namespace ofp::demux;

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
                            const ByteRange &data, UInt64 sessionID,
                            bool final) {
  UInt32 begin = end - UInt32_narrow_cast(data.size());

  if (!firstSeen_.valid()) {
    firstSeen_ = ts;
    end_ = begin;
  }

  log::debug("FlowState: receive segment", SegmentToString(begin, end, final));

  if (cache_.empty()) {
    if (finished_) {
      // Handle packets that arrive after flow is officially finished.
      log::warning("FlowState: drop late segment", SegmentToString(begin, end, final));
      return FlowData{sessionID};

    } else if (begin == end_) {
      // We have no data cached and this is the next data segment.
      lastSeen_ = ts;
      if (final) {
        finished_ = true;
      }
      log::debug("FlowState: return flow data", SegmentToString(begin, end, final));
      return FlowData{this, data, sessionID, false, final};
    }
  }

  if (Segment::lessThan(begin, end_)) {
    log::warning("FlowState: drop unexpected segment", SegmentToString(begin, end, final), "end is", end_);
    return FlowData{sessionID};
  }

  // Cache the data segment.
  cache_.store(end, data, final);

  lastSeen_ = ts;
  if (final) {
    finished_ = true;
  }

  return latestData(sessionID);
}

FlowData FlowState::latestData(UInt64 sessionID) {
  const Segment *seg = cache_.current();

  if (seg && seg->begin() == end_) {
    log::debug("FlowState: return flow data",
               SegmentToString(seg->begin(), seg->end(), seg->final()));
    return FlowData{this, seg->data(), sessionID, true, seg->final()};
  }

  return FlowData{sessionID};
}

void FlowState::addMissingData(size_t maxMissingBytes) {
  cache_.addMissingData(end_, maxMissingBytes);
}

void FlowState::clear() {
  firstSeen_.clear();
  lastSeen_.clear();
  end_ = 0;
  finished_ = false;
  if (!cache_.empty()) {
    log::warning("FlowState: clearing", cache_.cacheSize(), "bytes");
  }
  cache_.clear();
}

std::string FlowState::toString() const {
  // If there are no cached segments, return either "FIN" or "up".
  if (empty()) {
    return finished_ ? "FIN" : "up";
  } else {
    return cache_.toString();
  }
}
