// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_SEGMENTCACHE_H_
#define OFP_DEMUX_SEGMENTCACHE_H_

#include <vector>
#include "ofp/bytelist.h"

namespace ofp {
namespace demux {

OFP_BEGIN_IGNORE_PADDING

class Segment {
 public:
  Segment(UInt32 end, const ByteRange &data, bool final) : end_{end}, final_{final}, data_{data} {}

  UInt32 begin() const { return end_ - UInt32_narrow_cast(data_.size()); }
  UInt32 end() const { return end_; }
  ByteRange data() const { return data_.toRange(); }
  size_t size() const { return data_.size(); }
  bool final() const { return final_; }

  void append(const ByteRange &data, bool final) {
    assert(!final_);
    data_.add(data.data(), data.size());
    end_ += data.size();
    final_ = final;
  }

  void prepend(const ByteRange &data) {
    data_.insert(data_.begin(), data.data(), data.size());
  }

  void consume(size_t len) {
    assert(len <= data_.size());
    data_.remove(data_.begin(), len);
  }

  static bool lessThan(UInt32 lhs, UInt32 rhs);

 private:
  UInt32 end_ = 0;
  bool final_ = false;    // is this the last segment?
  ByteList data_;
};

OFP_END_IGNORE_PADDING

// Stores TCP segments that need to be reassembled.
class SegmentCache {
 public:
  // Store segment in the cache. If possible, combine this segment with
  // existing segments to fill in gaps.
  void store(UInt32 end, const ByteRange &data, bool final = false);

  // Return ptr to current segment. Return nullptr if there is no segment.
  const Segment *current() const;

  // Drain bytes from the current segment. When all bytes are drained, the
  // current segment is removed.
  void consume(size_t len);

  // Return number of segments.
  size_t size() const { return segments_.size(); }

  // Return true if cache is empty.
  bool empty() const { return segments_.empty(); }

  // Return description of cached segments for debugging.
  std::string toString() const;

  // Erase all data.
  void clear() { segments_.clear(); }

 private:
  std::vector<Segment> segments_;

  void append(UInt32 end, const ByteRange &data, bool final);
  void insert(UInt32 end, const ByteRange &data, size_t idx, bool final);
  void update(size_t idx, bool final);

  bool checkInvariant();
};

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_SEGMENTCACHE_H_
