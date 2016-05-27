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

  log::debug("FlowData::consume", len, "bytes");
  
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
                            bool final, Timestamp *lastSeen) {
  UInt32 begin = end - UInt32_narrow_cast(data.size());

  if (!started_) {
    end_ = begin;
    started_ = true;
  }

  log::debug("FlowState: receive segment", SegmentToString(begin, end, final));

  if (cache_.empty()) {
    if (finished_) {
      // Handle packets that arrive after flow is officially finished.
      log::warning("FlowState: drop late segment", SegmentToString(begin, end, final));
      return FlowData{sessionID};

    } else if (begin == end_) {
      // We have no data cached and this is the next data segment.
      *lastSeen = ts;
      if (final) {
        finished_ = true;
      }
      log::debug("FlowState: return flow data", SegmentToString(begin, end, final));
      return FlowData{this, data, sessionID, false, final};
    }
  }

  // Check if segment's end <= our `end_` value.
  if (!Segment::lessThan(end_, end)) {
    log::warning("FlowState: drop unexpected segment", SegmentToString(begin, end, final), "end is", end_);
    return FlowData{sessionID};
  }

  // Determine the data in the segment that is *new*.
  ByteRange newData;
  if (Segment::lessThan(begin, end_)) {
    log::warning("FlowState: overlapping segment detected", SegmentToString(begin, end, final), "end is", end_);
    newData = SafeByteRange(data.begin(), data.size(), end_ - begin);
  } else {
    newData = data;
  }

  // Cache the data segment.
  cache_.store(end, newData, final);

  *lastSeen = ts;
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
  end_ = 0;
  started_ = false;
  finished_ = false;
  if (!cache_.empty()) {
    log::warning("FlowState: clearing", cache_.cacheSize(), "bytes");
  }
  cache_.clear();
}

std::string FlowState::toString() const {
  // If there are no cached segments, return either "FIN" or "up".
  // If no segments have been seen at all, return "INIT". If there are cached
  // segments, return a description of the segment cache.
  if (empty()) {
    if (finished_) {
      return "FIN";
    } else if (started_) {
      return "up";
    } else {
      return "INIT";
    }
  } else {
    return cache_.toString();
  }
}
