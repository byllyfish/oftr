// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_FLOWSTATE_H_
#define OFP_DEMUX_FLOWSTATE_H_

#include "ofp/demux/segmentcache.h"
#include "ofp/timestamp.h"

namespace ofp {
namespace demux {

class FlowState;

OFP_BEGIN_IGNORE_PADDING

class FlowData {
 public:
  explicit FlowData(UInt64 sessionID) : sessionID_{sessionID} {}

  FlowData(FlowData &&other)
      : state_{other.state_},
        data_{other.data_},
        sessionID_{other.sessionID_},
        cached_{other.cached_},
        final_{other.final_} {
    other.state_ = nullptr;
  }

  const UInt8 *data() const { return data_.data(); }
  size_t size() const { return data_.size(); }
  UInt64 sessionID() const { return sessionID_; }
  bool final() const { return final_; }

  void consume(size_t len);

  FlowData(const FlowData &data) = delete;
  FlowData &operator=(const FlowData &data) = delete;

 private:
  FlowState *state_ = nullptr;
  ByteRange data_;
  UInt64 sessionID_ = 0;
  bool cached_ = false;
  bool final_ = false;

  // Private constructor for use by FlowState class.
  FlowData(FlowState *state, const ByteRange &data, UInt64 sessionID,
           bool cached, bool final)
      : state_{state},
        data_{data},
        sessionID_{sessionID},
        cached_{cached},
        final_{final} {}

  friend class FlowState;
};

class FlowState {
 public:
  FlowData receive(const Timestamp &ts, UInt32 end, const ByteRange &data,
                   UInt64 sessionID, bool final);

  const Timestamp &firstSeen() const { return firstSeen_; }
  const Timestamp &lastSeen() const { return lastSeen_; }
  UInt32 end() const { return end_; }
  bool empty() const { return cache_.empty(); }

  FlowData latestData(UInt64 sessionID);

  void clear();

 private:
  Timestamp firstSeen_;
  Timestamp lastSeen_;
  UInt32 end_ = 0;
  bool finished_ = false;
  SegmentCache cache_;

  void setFinished(UInt64 sessionID);

  friend class FlowData;
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_FLOWSTATE_H_
